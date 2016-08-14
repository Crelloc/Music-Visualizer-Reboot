#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <pthread.h>
#include "audioInformation.h"
#define FILE_PATH "/home/crelloc/Music/Em-Infinite.wav"


int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_AUDIO);

	struct Visualizer_Pkg visualizer_pkg_t;
	SDL_AudioSpec* wavSpec_ptr = &visualizer_pkg_t.wavSpec;
	Uint8** wavStart_ptr_ptr = &visualizer_pkg_t.AudioData_t.wavStart;
	Uint32* wavLength_ptr = &visualizer_pkg_t.AudioData_t.wavLength;

	if(SDL_LoadWAV(FILE_PATH, wavSpec_ptr, wavStart_ptr_ptr, wavLength_ptr) == NULL)
	{
		// TODO: Proper error handling
	
		return 1;
	}

	
	visualizer_pkg_t.AudioData_t.currentPos = *wavStart_ptr_ptr;
	visualizer_pkg_t.AudioData_t.currentLength = *wavLength_ptr;
	visualizer_pkg_t.AudioData_t.wavStart = *wavStart_ptr_ptr;
	visualizer_pkg_t.AudioData_t.wavLength = *wavLength_ptr;
	wavSpec_ptr->callback = MyAudioCallback;
	wavSpec_ptr->userdata = &visualizer_pkg_t;

	
	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, wavSpec_ptr, NULL,
			SDL_AUDIO_ALLOW_ANY_CHANGE);
	if(device == 0)
	{
		// TODO: Proper error handling
		
		return 1;
	}

	
	
	


	SDL_PauseAudioDevice(device, 0);

	while(visualizer_pkg_t.AudioData_t.currentLength > 0)
	{
		printf("%d\n", visualizer_pkg_t.AudioData_t.currentLength);
		SDL_Delay(100);
	}

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(*wavStart_ptr_ptr);
	SDL_Quit();
	return 0;
}
