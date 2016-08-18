#ifndef AUDIOINFORMATION_H
#define AUDIOINFORMATION_H

#include <SDL2/SDL.h>
#include <fftw3.h>
#include <stdlib.h>

struct AudioData;
struct Visualizer_Pkg;
struct FFTWop;
struct FFTW_Results;
typedef struct Visualizer_Pkg* Visualizer_Pkg_ptr;

void outputpowerspectrum(Visualizer_Pkg_ptr package);
void MyAudioCallback(void* userdata, Uint8* stream, int streamLength);
double Get8bitAudioSample(Uint8* bytebuffer,  SDL_AudioFormat format);
double Get16bitAudioSample(Uint8* bytebuffer, SDL_AudioFormat format);
double Get32bitAudioSample(Uint8* bytebuffer, SDL_AudioFormat format);

struct AudioData* GetAudioData(Visualizer_Pkg_ptr);
SDL_AudioSpec* GetSDL_AudioSpec(Visualizer_Pkg_ptr);
struct FFTW_Results* GetFFTW_Results(Visualizer_Pkg_ptr);
struct FFTWop* GetFFTWop(Visualizer_Pkg_ptr);


struct SoundInfo
{
	char** info;

}*g_SoundInfo_t;

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
	double* peakpower;
	double** peakmagMatrix;		//peakmagMatrix[channel][bucket]
	char*** outputMatrix;		//outputMatrix[channel][bucket][outputstring]
	double phase;
};

struct Visualizer_Pkg
{
	char* filename;
	int total_packets;
	int total_frames;
	int bitsize;

  	SDL_AudioDeviceID device;
	SDL_AudioSpec* wavSpec_ptr;                //SDL data type to analyze WAV file.
                                          //A structure that contains the audio output format.
	struct AudioData* AudioData_ptr;
	struct FFTW_Results* FFTW_Results_ptr;
	struct FFTWop* fftw_ptr;

	
	double (*GetAudioSample)(Uint8*, SDL_AudioFormat);
	void (*setupDFT)(Visualizer_Pkg_ptr, Uint8*, int );
}; 



#endif //AUDIOINFORMATION_H
