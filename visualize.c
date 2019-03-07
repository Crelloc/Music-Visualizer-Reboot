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

static SDL_AudioSpec* Verify_Audio_Spec(SDL_AudioSpec* wavSpec, SDL_AudioSpec have)
{

    if(wavSpec->format != have.format){
        printf("original sample format: %d\n"
        "new sample format: %d\n", wavSpec->format, have.format);
        wavSpec->format = have.format;
    }
    if(wavSpec->channels != have.channels){
        printf("original sample channels: %d\n"
        "new sample channels: %d\n", wavSpec->channels, have.channels);
        wavSpec->channels = have.channels;
    }
    if(wavSpec->samples != have.samples ){
        printf("original sample size: %d\n"
        "new sample size: %d\n", wavSpec->samples, have.samples);
        wavSpec->samples = have.samples;
    }
    return wavSpec;
}

static struct FFTWop* allocate_FFTWop(int channels, int frame_size)
{
    int i;
    struct FFTWop* fftw_ptr = malloc(channels*sizeof(struct FFTWop));
    if(!fftw_ptr) exit(EXIT_FAILURE);
    //allocating space for dft operations
    for(i=0; i<channels; ++i){
        fftw_ptr[i].in = fftw_malloc(sizeof(fftw_complex) * frame_size);
        if(!fftw_ptr[i].in) exit(EXIT_FAILURE);
        fftw_ptr[i].out = fftw_malloc(sizeof(fftw_complex) * frame_size);
        if(!fftw_ptr[i].out) exit(EXIT_FAILURE);
        fftw_ptr[i].index = i;
    }
    return fftw_ptr;
}


static struct FFTW_Results*  allocate_FFTW_Results(int channels, int totalpackets)
{
    int i, ch;
    struct FFTW_Results* FFTW_Results_ptr;

    FFTW_Results_ptr = malloc(totalpackets * sizeof(struct FFTW_Results));
    if(!(FFTW_Results_ptr)) exit(EXIT_FAILURE);;

    for (i = 0; i < totalpackets; ++i){
        //for peak results
        FFTW_Results_ptr[i].peakfreq = malloc(channels*sizeof(double));

        if(!FFTW_Results_ptr[i].peakfreq) exit(EXIT_FAILURE);

        FFTW_Results_ptr[i].peakpower = malloc(channels*sizeof(double));

        if(!FFTW_Results_ptr[i].peakpower) exit(EXIT_FAILURE);
        //for power spectrum (i.e. a double matrix) of
        //N BUCKETS that represent a frequency range
        FFTW_Results_ptr[i].peakmagMatrix = malloc(channels*sizeof(double));
        if(!FFTW_Results_ptr[i].peakmagMatrix) exit(EXIT_FAILURE);
        for(ch = 0; ch < channels ; ++ch){
            FFTW_Results_ptr[i].peakmagMatrix[ch] = malloc(BUCKETS*sizeof(double));
            if(!FFTW_Results_ptr[i].peakmagMatrix[ch]) exit(EXIT_FAILURE);
        }
    }
    return FFTW_Results_ptr;
}

static int Get_totalpackets(struct Visualizer_Pkg* vis_pkg, SDL_AudioSpec* wavSpec)
{
    int sizeof_packet;
    struct AudioData* audio;

    //size of samples in bytes?
    sizeof_packet =  vis_pkg->bitsize / 8 ;
    sizeof_packet *= wavSpec->channels;
    sizeof_packet *= wavSpec->samples;
    printf("buffer size per packet [bytes]: %d\n", sizeof_packet);

    //find the total number of packets
    //wavLength is the size of audio data in bytes
    audio = GetAudioData(vis_pkg);
    return (int)ceil((float)audio->wavLength/sizeof_packet);
}


static int Get_total_frames(struct Visualizer_Pkg* vis_pkg, int channels)
{
    struct AudioData* audio;
    audio = GetAudioData(vis_pkg);
    return audio->wavLength /= ((vis_pkg->bitsize/8) * channels);
}

static double (*set_audio_sample_function(int bitsize))(Uint8 *, SDL_AudioFormat)
{

    return  (bitsize == 8 ) ? Get8bitAudioSample
        :   (bitsize == 16) ? Get16bitAudioSample
        :   (bitsize == 32) ? Get32bitAudioSample
        :   NULL;
}


static void InitializeVariables(struct Visualizer_Pkg* vis_pkg, SDL_AudioSpec have,
                         SDL_AudioDeviceID device)
{
    SDL_AudioSpec* wavSpec;

    wavSpec           = Verify_Audio_Spec(GetSDL_AudioSpec(vis_pkg), have);
    vis_pkg->bitsize  = (int)SDL_AUDIO_BITSIZE(wavSpec->format);
    vis_pkg->GetAudioSample = set_audio_sample_function(vis_pkg->bitsize);
    assert(vis_pkg->GetAudioSample != NULL);

    vis_pkg->device   = device;
    vis_pkg->setupDFT = setupDFTForSound;
    vis_pkg->total_packets = Get_totalpackets(vis_pkg, wavSpec);


    //A frame can consist of N channels
    vis_pkg->total_frames = Get_total_frames(vis_pkg, wavSpec->channels);
    vis_pkg->frame_size = wavSpec->samples;

    //FFTW Results for each packet
    vis_pkg->FFTW_Results_ptr = allocate_FFTW_Results(wavSpec->channels, vis_pkg->total_packets);
    //EXIT_IF(ret==-1, device);
    vis_pkg->fftw_ptr = allocate_FFTWop(wavSpec->channels, vis_pkg->frame_size);
    //EXIT_IF(ret==-1, device);

    printf("%d bit data samples\n", vis_pkg->bitsize);
    printf("total packets: %d\n", vis_pkg->total_packets);
    printf("number of frames per packet: %d\n", vis_pkg->frame_size);
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
