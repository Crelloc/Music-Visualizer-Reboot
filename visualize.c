#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>
#include "audioInformation.h"
#include "dataprocessing.h"


#define FILE_PATH "/home/crelloc/Music/Classic_Beat.wav"


static volatile int keeprunning = 1;

void aborted(int sig)
{

	printf("\nAborted by signal: %d\n", sig);
	keeprunning = 0;

}



struct Visualizer_Pkg InitializePackage(SDL_AudioSpec* wavSpec,  Uint8* wavStart, Uint32 wavLength){

 	struct AudioData* AudioData_t = (struct AudioData*)malloc(sizeof(struct AudioData));

	AudioData_t->currentPos = wavStart;
	AudioData_t->wavStart = wavStart;
	AudioData_t->wavLength = wavLength;
	AudioData_t->currentLength = wavLength;
	wavSpec->callback = MyAudioCallback;
	wavSpec->userdata = AudioData_t;

  
	struct Visualizer_Pkg visualizer_pkg_t = {
						    .filename = FILE_PATH,
						    .AudioData_ptr = AudioData_t,
						    .wavSpec_ptr = wavSpec,
						    .FFTW_Results_ptr = NULL,
						    .GetAudioSample = NULL
						  };
  
  return visualizer_pkg_t;
}



int InitializeVariables(struct Visualizer_Pkg* vis_pkg){

	
	//initialize function ptr
	int whatformat = (int)SDL_AUDIO_BITSIZE(vis_pkg->wavSpec_ptr->format);

	switch(whatformat){
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

	//1 packet equals 4096 samples
	int samples_per_packet = vis_pkg->wavSpec_ptr->samples;

	//size of 4096 samples in bytes?
	int sizeof_packet =  whatformat * samples_per_packet / 8 ;

	//find the total number of packets
	//wavLength is the size of audio data in bytes
	Uint32 wavLength = vis_pkg->AudioData_ptr->wavLength;
	int totalpackets= (int)ceil((float)wavLength/sizeof_packet);
	vis_pkg->total_packets = totalpackets;

	//A frame can consist of N channels
	int channels = vis_pkg->wavSpec_ptr->channels;
	int total_frames = samples_per_packet / channels; 
	vis_pkg->total_frames = total_frames;

	//FFTW Results for each packet (4096 samples)
	//
	vis_pkg->FFTW_Results_ptr = (struct FFTW_Results*)malloc(totalpackets * sizeof(struct FFTW_Results)); 
	vis_pkg->fftw_ptr = (struct FFTWop*)malloc(channels * sizeof(struct FFTWop));

	//allocating space for dft operations
	for(int i=0; i<channels; ++i){
		vis_pkg->fftw_ptr[i].in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * total_frames);
		vis_pkg->fftw_ptr[i].out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * total_frames);
		vis_pkg->fftw_ptr[i].index = i;
	}

	printf("sizeofbuffer: %d\n", sizeof_packet);
	printf("totalframes: %d\n", total_frames);

	return sizeof_packet;
}



int main(int argc, char** argv)
{

	(void) signal(SIGINT, aborted);
	(void) signal(SIGTSTP, aborted);

	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

	SDL_AudioSpec wavSpec;
	Uint8* wavStart;
	Uint32 wavLength;


	if(SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL)
	{
		// TODO: Proper error handling

		return 1;
	}

	struct Visualizer_Pkg vis_pkg = InitializePackage(&wavSpec, wavStart, wavLength);


	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL,
		SDL_AUDIO_ALLOW_ANY_CHANGE);
	
	if(device == 0)	{
	// TODO: Proper error handling
		return 1;
	}

	int buffer_size = InitializeVariables(&vis_pkg);
	processWAVFile(wavLength, buffer_size, &vis_pkg);

	SDL_PauseAudioDevice(device, 0); //

	while((vis_pkg.AudioData_ptr->currentLength > 0) && keeprunning){

		SDL_Delay(100);
	}

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wavStart);
	SDL_Quit();

	return 0;
}
