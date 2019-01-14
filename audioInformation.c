#include "audioInformation.h"

extern volatile int keeprunning;
extern volatile int packet_pos;
extern volatile int print_spectrum;
extern const int BUCKETS;
static int g_endprogram = 0;

/*Function ENDNOW called only when stuck in SDL's
callback function: MyAudioCallback
*/

void ENDNOW(Uint8* wavStart, SDL_AudioDeviceID device){
    /*Note: we are not freeing the variables in the FFTW_Results struct
     we are going to let the OS do this for us after program has exited.
     */
    SDL_CloseAudioDevice(device);
    SDL_FreeWAV(wavStart);
    SDL_Quit();
    exit(0);
}

void outputpowerspectrum(Visualizer_Pkg_ptr package)
{
    int totchannels = GetSDL_AudioSpec(package)->channels;
    struct FFTW_Results* res= GetFFTW_Results(package);
    int thickness = 2, minutes, seconds, b, c, t, p, energy;
    double n_samples, tot_seconds;

    /*PRINT GRID*/
    if(system("clear") < 0){}

    for(c = 0; c < totchannels; ++c){
        for(b = 0; b < BUCKETS; ++b){
            if((c+1)%2 == 0)
                energy = res[packet_pos].peakmagMatrix[c][b];
            else
                energy = res[packet_pos].peakmagMatrix[c][BUCKETS-b-1];
            for(t = 0; t < thickness; ++t){
                for (p = 0; p < energy; ++p)
                    putchar('|');

                putchar('>');
                putchar('\n');
            }
        }
        putchar('\n');
        fflush(stdout);
    }

    /*OUTPUT REMAINING TIME*/
    n_samples = GetAudioData(package)->currentLength;
    n_samples /= (package->bitsize/8);
    n_samples /= totchannels;

    tot_seconds = n_samples / GetSDL_AudioSpec(package)->freq;
    minutes = tot_seconds / 60;
    seconds = (int)tot_seconds%60;

    printf("\n%02d:%02d\n", minutes, seconds);
    fflush(stdout);
}

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{
    struct Visualizer_Pkg* package = (struct Visualizer_Pkg*)userdata;
    struct AudioData* audio= GetAudioData(package);
    Uint32 length;

    if(audio->currentLength == 0){
        if(g_endprogram)
            ENDNOW(audio->wavStart, package->device);
        SDL_memset(stream, 0, streamLength);  // just silence.
        g_endprogram=1;
        return;

    }
    outputpowerspectrum(package);

    length = (Uint32)streamLength;
    length = (length > audio->currentLength ? audio->currentLength : length);

    SDL_memcpy(stream, audio->currentPos, length);

    audio->currentPos += length;
    audio->currentLength -= length;

    packet_pos++;
}

/*function Get8bitAudioSample hasnt been implemented*/
double Get8bitAudioSample(Uint8* bytebuffer,SDL_AudioFormat format)
{
    return 0.0;
}

double Get16bitAudioSample(Uint8* bytebuffer, SDL_AudioFormat format)
{

    Uint16 val =  0x0;

    if(SDL_AUDIO_ISLITTLEENDIAN(format))
        val = (uint16_t)bytebuffer[0] | ((uint16_t)bytebuffer[1] << 8);

    else
        val = ((uint16_t)bytebuffer[0] << 8) | (uint16_t)bytebuffer[1];

    if(SDL_AUDIO_ISSIGNED(format))
        return ((int16_t)val)/32768.0;

    return val/65535.0;
}
/*function Get32bitAudioSample hasnt been implemented*/

double Get32bitAudioSample(Uint8* bytebuffer, SDL_AudioFormat format)
{
    return 0.0;
}

struct AudioData* GetAudioData(Visualizer_Pkg_ptr package)
{
    return package->AudioData_ptr;
}

SDL_AudioSpec* GetSDL_AudioSpec(Visualizer_Pkg_ptr package)
{
    return package->wavSpec_ptr;
}

struct FFTW_Results* GetFFTW_Results(Visualizer_Pkg_ptr package)
{
    return package->FFTW_Results_ptr;
}

struct FFTWop* GetFFTWop(Visualizer_Pkg_ptr package)
{
    return package->fftw_ptr;
}
