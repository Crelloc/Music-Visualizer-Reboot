#include "audioInformation.h"
//#include <SDL2/SDL.h>


void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{

	struct AudioData* audio= (struct AudioData*)userdata;

	if(audio->currentLength == 0)  
	    return;
	   
	Uint32 length = (Uint32)streamLength;
	length = (length > audio->currentLength ? audio->currentLength : length);

	SDL_memcpy(stream, audio->currentPos, length);

	audio->currentPos += length;
	audio->currentLength -= length;



}

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

double Get32bitAudioSample(Uint8* bytebuffer, SDL_AudioFormat format)
{
	return 0.0;
}

struct AudioData GetAudioData(Visualizer_Pkg_ptr package)
{
	return *package->AudioData_ptr;
}

SDL_AudioSpec GetSDL_AudioSpec(Visualizer_Pkg_ptr package)
{
	return *package->wavSpec_ptr;
}

struct FFTW_Results GetFFTW_Results(Visualizer_Pkg_ptr package)
{
	return *package->FFTW_Results_ptr;
}

struct FFTWop GetFFTWop(Visualizer_Pkg_ptr package)
{
	return *package->fftw_ptr;
}