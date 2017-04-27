#include "dataprocessing.h"
#include <assert.h> 
#include <math.h>

enum {re, im}; //real and imaginary
extern volatile int keeprunning;
extern const int BUCKETS;


void setupDFTForSound(Visualizer_Pkg_ptr vis_pkg_ptr, Uint8* buffer, int bytesRead)
{
    int bytewidth = vis_pkg_ptr->bitsize / 8;
    SDL_AudioSpec* wavSpec = GetSDL_AudioSpec(vis_pkg_ptr);
    int channels = wavSpec->channels;
    SDL_AudioFormat fmt = wavSpec->format;
    int frames = bytesRead / (bytewidth * channels);
    struct FFTWop* fftwop = GetFFTWop(vis_pkg_ptr);
    int count = 0, c;

    for(c = 0; c < channels; ++c){
        fftwop[c].p = fftw_plan_dft_1d(frames, fftwop[c].in,
                fftwop[c].out, FFTW_FORWARD, FFTW_MEASURE);
    }

    while(count < frames){
        for(c = 0; c< channels; ++c){
            fftwop[c].in[count][re] = vis_pkg_ptr->GetAudioSample(buffer, fmt);
            fftwop[c].in[count][im] = 0.0;

            buffer+=bytewidth;
        }
        count++;
    }
}

int getFileSize(FILE *inFile)
{
    int fileSize = 0;
    fseek(inFile,0,SEEK_END);

    fileSize=ftell(inFile);

    fseek(inFile,0,SEEK_SET);

    return fileSize;
}

void processWAVFile(Uint32 wavLength, int buffer_size, Visualizer_Pkg_ptr vis_pkg_ptr)
{
    struct FFTWop* dft = GetFFTWop(vis_pkg_ptr);
    int channels = GetSDL_AudioSpec(vis_pkg_ptr)->channels;

    FILE* wavFile = fopen(vis_pkg_ptr->filename, "r");
    int filesize = getFileSize(wavFile);

    Uint8* buffer = (Uint8*)malloc(buffer_size*sizeof(Uint8));

    size_t bytesRead;
    int packet_index = 0, i;
    //Skip header information in .WAV file
    bytesRead = fread(buffer, sizeof buffer[0], filesize-wavLength, wavFile);

        //Reading actual audio data
    while ((bytesRead = fread(buffer, sizeof buffer[0],
        buffer_size/sizeof(buffer[0]), wavFile)) > 0 && keeprunning){

        vis_pkg_ptr->setupDFT(vis_pkg_ptr, buffer, bytesRead);
        for(i = 0; i < channels; ++i){

            fftw_execute(dft[i].p);
            analyze_FFTW_Results(vis_pkg_ptr, dft[i], packet_index, i ,bytesRead);
            fftw_destroy_plan(dft[i].p);
        }
        packet_index++;
    }

    /*MEMORY MANAGEMENT*/
    free(buffer);
    for(i = 0; i<channels; ++i){

        free(dft[i].in);
        free(dft[i].out);
    }
    free(dft);
    fclose(wavFile);
}

void analyze_FFTW_Results(Visualizer_Pkg_ptr packet, struct FFTWop fftwop ,
                                        int packet_index, int ch,size_t bytesRead)
{

    double real, imag; 
    double peakmax = 1.7E-308 ;
    int max_index = -1, i, j;
    double magnitude;
    double* peakmaxArray = (double*)malloc(BUCKETS*sizeof(double));
    double nyquist = packet->wavSpec_ptr->freq / 2;
    double freq_bin[] = {19.0, 140.0, 400.0, 2600.0, 5200.0, nyquist };

    SDL_AudioSpec* wavSpec = GetSDL_AudioSpec(packet);

    int frames = bytesRead / (wavSpec->channels * packet->bitsize / 8);
    struct FFTW_Results* results = GetFFTW_Results(packet);


    for(i = 0; i<BUCKETS; ++i) peakmaxArray[i] = 1.7E-308;

    for(j = 0; j < frames/2; ++j){

        real =  fftwop.out[j][0];
        imag =  fftwop.out[j][1];
        magnitude = sqrt(real*real+imag*imag);
        double freq = j * (double)wavSpec->freq / frames;

        for (i = 0; i < BUCKETS; ++i){
            if((freq>freq_bin[i]) && (freq <=freq_bin[i+1])){
                if (magnitude > peakmaxArray[i]){
                    peakmaxArray[i] = magnitude;
                }
            }
        }

        if(magnitude > peakmax){
                peakmax = magnitude;
                max_index = j;
        }
    }

    results[packet_index].peakpower[ch] =  10*(log10(peakmax));
    results[packet_index].peakfreq[ch] = max_index*(double)wavSpec->freq/frames;

    for(i = 0; i< BUCKETS; ++i){
        results[packet_index].peakmagMatrix[ch][i]=10*(log10(peakmaxArray[i]));
    }

    free(peakmaxArray);
}
