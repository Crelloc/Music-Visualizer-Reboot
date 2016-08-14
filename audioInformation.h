#ifndef AUDIOINFORMATION_H
#define AUDIOINFORMATION_H


#include <SDL2/SDL.h>
#include <fftw3.h>

struct AudioData
{
	Uint8* currentPos;
	Uint32 currentLength;
	Uint8* wavStart;                                        //pointer to audio data 
  	Uint32 wavLength;                                       //length of audio data
};


struct FFTW_Results
{
    
};

struct Visualizer_Pkg
{
  	SDL_AudioDeviceID device;
	struct AudioData AudioData_t;
	SDL_AudioSpec wavSpec;                //SDL data type to analyze WAV file.
                                          //A structure that contains the audio output format.
	struct FFTW_Results* FFTW_Results_t;
	char* filename;

};

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength);

#endif
