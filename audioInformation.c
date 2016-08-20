#include "audioInformation.h"

extern volatile int keeprunning;
extern volatile int packet_pos;
extern volatile int print_spectrum;
extern const int BUCKETS;



void outputpowerspectrum(Visualizer_Pkg_ptr package){

	if(system("clear") < 0){//handle error
			
	}
	int totchannels = package->wavSpec_ptr->channels;
	struct FFTW_Results* res= GetFFTW_Results(package);
	
	for(int c= 0; c< totchannels; ++c){
		int energy;
		if(totchannels > 1 && ((c+1)%2 == 0)){
			for(int b=0; b < BUCKETS; ++b){
				energy = res[packet_pos].peakmagMatrix[c][b];
				for(int t=0; t<2; ++t){
					for (int p = 0; p< energy; ++p){
								
						putchar('|');
					}
					putchar('>');
					putchar('\n');

				}
			}
			
		}
		else{
			for(int b=0; b < BUCKETS; ++b){
				energy = res[packet_pos].peakmagMatrix[c][BUCKETS-b-1];
				for(int t=0; t<2; ++t){
					for (int p = 0; p< energy; ++p){
								
						putchar('|');
					}
					putchar('>');
					putchar('\n');

				}
			}
			
		}
		putchar('\n');
		fflush(stdout);

	}


	packet_pos++;
}

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{
	struct Visualizer_Pkg* package = (struct Visualizer_Pkg*)userdata;
	struct AudioData* audio= GetAudioData(package);

	if(audio->currentLength == 0)  
	    return;
	
	outputpowerspectrum(package);

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