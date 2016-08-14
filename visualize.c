#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <pthread.h>
#include "audioInformation.h"
#define FILE_PATH "/home/crelloc/Music/Em-Infinite.wav"


int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec wavSpec;
	Uint8* wavStart;
	Uint32 wavLength;

	if(SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL)
	{
		// TODO: Proper error handling
	
		return 1;
	}

	struct AudioData audio;
	audio.currentPos = wavStart;
	audio.currentLength = wavLength;
	audio.wavStart = wavStart;
	audio.wavLength = wavLength;
	wavSpec.callback = MyAudioCallback;
	wavSpec.userdata = &audio;

	
	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL,
			SDL_AUDIO_ALLOW_ANY_CHANGE);
	if(device == 0)
	{
		// TODO: Proper error handling
		
		return 1;
	}

	struct Visualizer_Pkg visualizer_pkg_t;
	visualizer_pkg_t.AudioData_t = audio;
	visualizer_pkg_t.wavSpec = wavSpec;
	visualizer_pkg_t.device = device;

	SDL_PauseAudioDevice(device, 0);

	while(visualizer_pkg_t.AudioData_t.currentLength > 0)
	{
		SDL_Delay(100);
	}

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wavStart);
	SDL_Quit();
	return 0;
}
