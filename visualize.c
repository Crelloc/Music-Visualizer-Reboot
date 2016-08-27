#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <getopt.h>
#include <errno.h>  //stderr() && strerror()
#include <unistd.h> //getopt()
#include "audioInformation.h"
#include "dataprocessing.h"




//#define FILE_PATH "/home/crelloc/Music/Em-Infinite.wav"


volatile int keeprunning = 1;
volatile int packet_pos = 0;
volatile int print_spectrum = 0;
static volatile int time_to_exit = 0;
const int BUCKETS = 5;
char FILE_PATH[500] = {0};



void aborted(int sig){

	printf("\nAborted by signal: %d\n", sig);
	keeprunning = 0;


}


void InitializePackage(SDL_AudioSpec* wavSpec,  Uint8* wavStart, Uint32 wavLength,
							Visualizer_Pkg_ptr* vis_pkg)
{

 	struct AudioData* AudioData_t = (struct AudioData*)
 					malloc(sizeof(struct AudioData));

	AudioData_t->currentPos = wavStart;
	AudioData_t->wavStart = wavStart;
	AudioData_t->wavLength = wavLength;
	AudioData_t->currentLength = wavLength;

	wavSpec->callback = MyAudioCallback;
  	wavSpec->userdata = *vis_pkg;

	
	(*vis_pkg)->filename = FILE_PATH;
	(*vis_pkg)->AudioData_ptr = AudioData_t;
	(*vis_pkg)->wavSpec_ptr = wavSpec;
	(*vis_pkg)->FFTW_Results_ptr = NULL;
	(*vis_pkg)->GetAudioSample = NULL;

  
 
}



void InitializeVariables(struct Visualizer_Pkg* vis_pkg, SDL_AudioSpec have){

	SDL_AudioSpec* wavSpec = GetSDL_AudioSpec(vis_pkg);
	//initialize function ptr
	if(wavSpec->format != have.format)
		wavSpec->format = have.format;
	
	int bitsize = (int)SDL_AUDIO_BITSIZE(wavSpec->format);

	switch(bitsize){
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

	vis_pkg->bitsize = bitsize;

	if(wavSpec->channels != have.channels)
		wavSpec->channels = have.channels;
	
	int ch = (int)wavSpec->channels;
	switch(ch){
		
		case 1: 
			printf("output channels: %d (mono)\n", ch);
			vis_pkg->setupDFT = setupDFTForMono;
			break;
		case 2:
			printf("output channels: %d (stereo)\n", ch);
			vis_pkg->setupDFT = setupDFTForStereo;
			break;
		case 4:
			//vis_pkg->setupDFT =
			printf("output channels: %d (quad)\n", ch);
			break;
		case 6:
			//vis_pkg->setupDFT =
			printf("output channels: %d (5.1)\n", ch);
			break;
		default:
			break;

	}

	if(wavSpec->samples != have.samples ){
		printf("original sample size: %d\n"
		"new sample size: %d\n", wavSpec->samples, have.samples);
		wavSpec->samples = have.samples;
	}

	//size of samples in bytes?
	int sizeof_packet =  bitsize / 8 ;
	sizeof_packet *=wavSpec->channels;
	sizeof_packet *= wavSpec->samples;
	
// 	assert(sizeof_packet == (int)have.size
// 		&& "buffer length calculation is equal to SDL's .size calculation"
// 	);

	//find the total number of packets
	//wavLength is the size of audio data in bytes
	struct AudioData* audio = GetAudioData(vis_pkg);
	
	int totalpackets= (int)ceil((float)audio->wavLength/sizeof_packet);
	vis_pkg->total_packets = totalpackets;

	//A frame can consist of N channels
	int frame_size = wavSpec->samples; 
	vis_pkg->frame_size = frame_size;
	vis_pkg->total_frames = audio->wavLength/((bitsize/8) * wavSpec->channels);
	//FFTW Results for each packet 
	//

	vis_pkg->FFTW_Results_ptr = (struct FFTW_Results*)
			malloc(totalpackets * sizeof(struct FFTW_Results));

		
	for (int j = 0; j < totalpackets; ++j){
		//for peak results
		vis_pkg->FFTW_Results_ptr[j].peakfreq = (double*)
					malloc(wavSpec->channels*sizeof(double));
		vis_pkg->FFTW_Results_ptr[j].peakpower = (double*)
					malloc(wavSpec->channels*sizeof(double));

		//for power spectrum (i.e. a double matrix) of 
			//N BUCKETS that represent a frequency range
		vis_pkg->FFTW_Results_ptr[j].peakmagMatrix = (double**)
						malloc(wavSpec->channels*sizeof(double));
		for(int ch = 0; ch < wavSpec->channels ; ++ch){
			vis_pkg->FFTW_Results_ptr[j].peakmagMatrix[ch] = (double*)
						malloc(BUCKETS*sizeof(double));
		}

	}
	vis_pkg->fftw_ptr = (struct FFTWop*)malloc(wavSpec->channels * sizeof(struct FFTWop));

	//allocating space for dft operations
	for(int i=0; i<wavSpec->channels; ++i){
		vis_pkg->fftw_ptr[i].in = (fftw_complex*) 
			fftw_malloc(sizeof(fftw_complex) * frame_size);
		vis_pkg->fftw_ptr[i].out = (fftw_complex*) 
			fftw_malloc(sizeof(fftw_complex) * frame_size);
		vis_pkg->fftw_ptr[i].index = i;
	}

	printf("buffer size per packet [bytes]: %d\n", sizeof_packet);
	printf("number of frames per packet: %d\n", frame_size);
	printf("total frames: %d\n", vis_pkg->total_frames);
	printf("FILE_PATH: %s\n", vis_pkg->filename);
	printf("\nPress ENTER to continue:\n");
	fflush(stdout);
	while(getchar() != 0xa && keeprunning);
	
}



int main(int argc, char** argv)
{

	int opt;
	struct option longopts[] = {
		{"file",	1,	NULL,	'f'}, 
		{0,0,0,0}	
	};

	while((opt = getopt_long(argc, argv, ":f:", longopts, NULL)) != -1){
		switch(opt){
		case 'f':
			FILE_PATH = optarg;
			break;
		case ':':
			printf("option needs a value\n");
			break;
		case '?':
			printf("unknown option: %c\n", optopt);
			break;	
		}
	}	
	(void) signal(SIGINT, aborted);
	(void) signal(SIGTSTP, aborted);

	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

	SDL_AudioSpec wavSpec, have;
	Uint8* wavStart;
	Uint32 wavLength;


	if(SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL)
	{
		// TODO: Proper error handling

		return 1;
	}

	struct Visualizer_Pkg* vis_pkg = (struct Visualizer_Pkg*)
				malloc(sizeof(struct Visualizer_Pkg));
	InitializePackage(&wavSpec, wavStart, wavLength, &vis_pkg);


	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, &have,
		SDL_AUDIO_ALLOW_ANY_CHANGE);
	
	if(device == 0)	{
	// TODO: Proper error handling
		return 1;
	}

	InitializeVariables(vis_pkg, have);
	processWAVFile(wavLength, have.size ,vis_pkg);



	SDL_PauseAudioDevice(device, 0); //play song

	while((vis_pkg->AudioData_ptr->currentLength > 0) && keeprunning){

		SDL_Delay(100);
	}
	

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wavStart);
	SDL_Quit();
	return 0;
}

