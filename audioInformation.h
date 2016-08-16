#ifndef AUDIOINFORMATION_H
#define AUDIOINFORMATION_H

#include <SDL2/SDL.h>
#include <fftw3.h>
#include <stdlib.h>

struct AudioData
{
	Uint8* currentPos;
	Uint32 currentLength;
	Uint8* wavStart;                                        //pointer to audio data 
  	Uint32 wavLength;                                       //length of audio data
};

struct FFTWop
{
    fftw_complex *in;                 //used for channel data before fftw operation
    fftw_complex *out;                //will contain real and imaginary data for left and right channel after fftw operation.
    fftw_plan p;                    //fftw_plan is a fftw3 data type that allocates memory for fftw
                                          //read the '2.3 One-Dimensional DFTs of Real Data' section for more information:
                                        //http://www.fftw.org/#documentation
    int index;
    
};

struct FFTW_Results
{
	double* peakfreq;
    double* peakmag;
    double** peakmagMatrix;		//peakmagMatrix[channel][bucket]
    char*** outputMatrix;		//outputMatrix[channel][bucket][outputstring]

};

struct Visualizer_Pkg
{
	char* filename;

  	SDL_AudioDeviceID device;
	struct AudioData* AudioData_ptr;
	SDL_AudioSpec* wavSpec_ptr;                //SDL data type to analyze WAV file.
                                          //A structure that contains the audio output format.
	struct FFTW_Results* FFTW_Results_ptr;
	struct FFTWop* fftw_ptr;
	double (*GetAudioSample)(Uint8** bytebuffer, SDL_AudioFormat format);

}; 

typedef struct Visualizer_Pkg *Visualizer_Pkg_ptr;


void MyAudioCallback(void* userdata, Uint8* stream, int streamLength);

double Get8bitAudioSample(Uint8** bytebuffer,SDL_AudioFormat format);
double Get16bitAudioSample(Uint8** bytebuffer, SDL_AudioFormat format);
double Get32bitAudioSample(Uint8** bytebuffer, SDL_AudioFormat format);

#endif
