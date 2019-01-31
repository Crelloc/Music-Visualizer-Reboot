#define _POSIX_C_SOURCE 2
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <errno.h>
#include "audioInformation.h"
#include "dataprocessing.h"

#define EXIT_IF(EXP, device) \
do { if (EXP){\
        fprintf(stderr, "Failed to allocate memory!\n"); \
        SDL_CloseAudioDevice(device); \
        SDL_Quit(); \
        exit(EXIT_FAILURE);} } \
while (0)

/*GLOBAL VARIABLES*/
volatile int keeprunning = 1;
volatile int packet_pos = 0;
volatile int print_spectrum = 0;
static volatile int time_to_exit = 0;
const int BUCKETS = 5;
char* FILE_PATH;

static int InitializePackage(SDL_AudioSpec* wavSpec,  Uint8* wavStart, Uint32 wavLength,
                            Visualizer_Pkg_ptr vis_pkg)
{
    int ret = 0;
    struct AudioData* AudioData_t = malloc(sizeof(struct AudioData));
    if(!AudioData_t){
        fprintf(stderr, "Failed to allocate memory\n");
        ret = 1;
        return ret;
    }
    AudioData_t->currentPos = wavStart;
    AudioData_t->wavStart = wavStart;
    AudioData_t->wavLength = wavLength;
    AudioData_t->currentLength = wavLength;

    wavSpec->callback = MyAudioCallback;
    wavSpec->userdata = &(*vis_pkg);

    vis_pkg->filename = FILE_PATH;
    vis_pkg->AudioData_ptr = AudioData_t;
    vis_pkg->wavSpec_ptr = wavSpec;
    return ret;
}

static void InitializeVariables(struct Visualizer_Pkg* vis_pkg, SDL_AudioSpec have,
                         SDL_AudioDeviceID device)
{
    int sizeof_packet, totalpackets, frame_size, i, ch;
    struct AudioData* audio;

    vis_pkg->device = device;
    vis_pkg->setupDFT = setupDFTForSound;

    SDL_AudioSpec* wavSpec = GetSDL_AudioSpec(vis_pkg);
    //initialize function ptr
    if(wavSpec->format != have.format)
        wavSpec->format = have.format;

    vis_pkg->bitsize = (int)SDL_AUDIO_BITSIZE(wavSpec->format);

    if(wavSpec->channels != have.channels)
        wavSpec->channels = have.channels;

    if(wavSpec->samples != have.samples ){
        printf("original sample size: %d\n"
        "new sample size: %d\n", wavSpec->samples, have.samples);
        wavSpec->samples = have.samples;
    }

    //size of samples in bytes?
    sizeof_packet =  vis_pkg->bitsize / 8 ;
    sizeof_packet *=wavSpec->channels;
    sizeof_packet *= wavSpec->samples;

    //find the total number of packets
    //wavLength is the size of audio data in bytes
    audio = GetAudioData(vis_pkg);

    totalpackets= (int)ceil((float)audio->wavLength/sizeof_packet);
    vis_pkg->total_packets = totalpackets;

    //A frame can consist of N channels
    frame_size = wavSpec->samples;
    vis_pkg->frame_size = frame_size;
    vis_pkg->total_frames = audio->wavLength;
    vis_pkg->total_frames /= ((vis_pkg->bitsize/8) * wavSpec->channels);
    //FFTW Results for each packet
    //

    vis_pkg->FFTW_Results_ptr = malloc(totalpackets * sizeof(struct FFTW_Results));
    EXIT_IF(!vis_pkg->FFTW_Results_ptr, device);

    for (i = 0; i < totalpackets; ++i){
        //for peak results
        vis_pkg->FFTW_Results_ptr[i].peakfreq = malloc(wavSpec->channels*sizeof(double));

        EXIT_IF(!vis_pkg->FFTW_Results_ptr[i].peakfreq, device);

        vis_pkg->FFTW_Results_ptr[i].peakpower = malloc(wavSpec->channels*sizeof(double));

        EXIT_IF(!vis_pkg->FFTW_Results_ptr[i].peakpower, device);
        //for power spectrum (i.e. a double matrix) of
        //N BUCKETS that represent a frequency range
        vis_pkg->FFTW_Results_ptr[i].peakmagMatrix = malloc(wavSpec->channels*sizeof(double));
        EXIT_IF(!vis_pkg->FFTW_Results_ptr[i].peakmagMatrix, device);
        for(ch = 0; ch < wavSpec->channels ; ++ch){
            vis_pkg->FFTW_Results_ptr[i].peakmagMatrix[ch] = malloc(BUCKETS*sizeof(double));
            EXIT_IF(!vis_pkg->FFTW_Results_ptr[i].peakmagMatrix[ch], device);
        }

    }
    vis_pkg->fftw_ptr = malloc(wavSpec->channels*sizeof(struct FFTWop));
    EXIT_IF(!vis_pkg->fftw_ptr, device);
    //allocating space for dft operations
    for(i=0; i<wavSpec->channels; ++i){
        vis_pkg->fftw_ptr[i].in = fftw_malloc(sizeof(fftw_complex) * frame_size);
        EXIT_IF(!vis_pkg->fftw_ptr[i].in, device);
        vis_pkg->fftw_ptr[i].out = fftw_malloc(sizeof(fftw_complex) * frame_size);
        EXIT_IF(!vis_pkg->fftw_ptr[i].out, device);
        vis_pkg->fftw_ptr[i].index = i;
    }

    switch(vis_pkg->bitsize){
        case 8:
            printf("8 bit data samples\n");
            vis_pkg->GetAudioSample = Get8bitAudioSample;
            break;
        case 16:
            printf("16 bit data samples\n");
            vis_pkg->GetAudioSample = Get16bitAudioSample;
            break;
        case 32:
            vis_pkg->GetAudioSample = Get32bitAudioSample;
            printf("32 bit data samples\n");
            break;
        default:
            break;
    }
    printf("total packets: %d\n", totalpackets);
    printf("buffer size per packet [bytes]: %d\n", sizeof_packet);
    printf("number of frames per packet: %d\n", frame_size);
    printf("total frames: %d\n", vis_pkg->total_frames);
    printf("FILE_PATH: %s\n", vis_pkg->filename);
    printf("\nPress ENTER to continue:\n");
    fflush(stdout);

    while(getchar() != 0xa && keeprunning);
}


/*TRAP FUNCTION*/
static void aborted(int sig)
{
    printf("\nAborted by signal: %d\n", sig);
    keeprunning = 0;
}

int main(int argc, char** argv)
{

    int opt, i, j;
    SDL_AudioSpec wavSpec, have;
    SDL_AudioDeviceID device;
    Uint8* wavStart;
    Uint32 wavLength;
    struct Visualizer_Pkg* vis_pkg;

    while((opt = getopt(argc, argv, ":f:")) != -1){

        switch(opt){
            case 'f':
                FILE_PATH = optarg;
                break;
            case ':':
                printf("option needs a value\n");
                goto usage;
            case '?':
                printf("unknown option: %c\n", optopt);
                goto usage;
            default:
usage:          printf("usage %s [-f] \'PATH/TO/FILE\']\n",argv[0]);
                return 1;
        }
    }
    if (optind != argc) goto usage;

    (void) signal(SIGINT, aborted);
    (void) signal(SIGTSTP, aborted);

    SDL_Init(SDL_INIT_AUDIO);
    if(SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL)
    {
        // TODO: Proper error handling
        SDL_Log("Failed to load wav file: %s", SDL_GetError());
        return 1;
    }

    vis_pkg = malloc(sizeof(struct Visualizer_Pkg));
    if(!vis_pkg) goto EXIT;
    i = InitializePackage(&wavSpec, wavStart, wavLength, vis_pkg);
    if(i) goto EXIT;

    device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, &have,
                                    SDL_AUDIO_ALLOW_ANY_CHANGE);

    if(device == 0){
        SDL_Log("Failed to open audio: %s", SDL_GetError());
        return 1;
    }

    InitializeVariables(vis_pkg, have, device);
    processWAVFile(wavLength, have.size ,vis_pkg);

    SDL_PauseAudioDevice(device, 0); //play song

    while(GetAudioData(vis_pkg)->currentLength > 0 && keeprunning);


    //Free vis_pkg data
    for(i=0; i<vis_pkg->total_packets; ++i){
        free(vis_pkg->FFTW_Results_ptr[i].peakfreq);
        free(vis_pkg->FFTW_Results_ptr[i].peakpower);
        for(j=0; j<have.channels; ++j){
            free(vis_pkg->FFTW_Results_ptr[i].peakmagMatrix[j]);
        }
        free(vis_pkg->FFTW_Results_ptr[i].peakmagMatrix);
    }
    free(vis_pkg->FFTW_Results_ptr);

    SDL_CloseAudioDevice(device);
EXIT:
    SDL_FreeWAV(wavStart);
    SDL_Quit();

    printf("goodbye.\n");
    return 0;
}
