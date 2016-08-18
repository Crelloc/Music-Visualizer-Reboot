#include "dataprocessing.h"
#include <assert.h> 
#include <math.h>

enum {left, right}; //left and right channels
enum {re, im};	//real and imaginary
extern volatile int keeprunning;
extern const int BUCKETS;
const int BINS = 5;

void setupDFTForStereo(Visualizer_Pkg_ptr vis_pkg_ptr, Uint8* buffer, int bytesRead)
{	
	int frames = bytesRead / (2 * vis_pkg_ptr->wavSpec_ptr->channels);

	struct FFTWop* fftwop = GetFFTWop(vis_pkg_ptr);
	
	fftwop[0].p = fftw_plan_dft_1d(frames, fftwop[0].in, fftwop[0].out, FFTW_FORWARD, FFTW_MEASURE);
        fftwop[1].p = fftw_plan_dft_1d(frames, fftwop[1].in, fftwop[1].out, FFTW_FORWARD, FFTW_MEASURE);
	 
	int count = 0;
	while(count < frames){

		fftwop[left].in[count][re] = vis_pkg_ptr->GetAudioSample(buffer, vis_pkg_ptr->wavSpec_ptr->format);
		fftwop[left].in[count][im] = 0.0;

		buffer+=2;

		fftwop[right].in[count][re] = vis_pkg_ptr->GetAudioSample(buffer, vis_pkg_ptr->wavSpec_ptr->format);
		fftwop[right].in[count][im] = 0.0;

		buffer+=2;
		count++;
	}

	assert(count == frames && 
		"converted the correct amount of bytes.");

}


int getFileSize(FILE *inFile)
{
	int fileSize = 0;
	fseek(inFile,0,SEEK_END);

	fileSize=ftell(inFile);

	fseek(inFile,0,SEEK_SET);

	return fileSize;
}

void processWAVFile(Uint32 wavLength, int buffer_size, Visualizer_Pkg_ptr vis_pkg_ptr){

	printf("FILENAME: %s\n", vis_pkg_ptr->filename);

	FILE* wavFile = fopen(vis_pkg_ptr->filename, "r");
    	int filesize = getFileSize(wavFile);

    	Uint8* buffer = (Uint8*)malloc(buffer_size*sizeof(Uint8));

    	size_t bytesRead;
    	int packet_index = 0;



    	//Skip header information in .WAV file
    	bytesRead = fread(buffer, sizeof buffer[0], filesize-wavLength, wavFile);
  

    	//Reading actual audio data
	while ((bytesRead = fread(buffer, sizeof buffer[0], buffer_size/sizeof(buffer[0]), 
		wavFile)) > 0 && keeprunning){

		vis_pkg_ptr->setupDFT(vis_pkg_ptr, buffer, bytesRead);
		for(int i=0; i < vis_pkg_ptr->wavSpec_ptr->channels; ++i){

			fftw_execute(vis_pkg_ptr->fftw_ptr[i].p);
			analyze_FFTW_Results(vis_pkg_ptr, GetFFTWop(vis_pkg_ptr)[i], packet_index, i ,bytesRead);
			fftw_destroy_plan(vis_pkg_ptr->fftw_ptr[i].p);


		}
		packet_index++;		
	} 
	assert(packet_index == vis_pkg_ptr->total_packets &&
		"correct number of packets analyzed");

		
	free(buffer);
	for(int i=0; i<vis_pkg_ptr->wavSpec_ptr->channels; ++i){

		free(vis_pkg_ptr->fftw_ptr[i].in);
		free(vis_pkg_ptr->fftw_ptr[i].out);

	}
	
	free(vis_pkg_ptr->fftw_ptr);
	buffer = NULL;
	vis_pkg_ptr->fftw_ptr = NULL;



	fclose(wavFile);
}

void analyze_FFTW_Results(Visualizer_Pkg_ptr packet, struct FFTWop fftwop ,int packet_index, int ch,size_t bytesRead)
{

	double re, im, phase; 
  	double peakmax = 1.7E-308 ;
 	double peakfreq = 0.0;
  	int max_index = -1;
  	double magnitude;
  	double* peakmaxArray = (double*)malloc(BUCKETS*sizeof(double));
  	double nyquist = packet->wavSpec_ptr->freq / 2;
  	double freq_bin[] = {19.0, 140.0, 400.0, 2600.0, 5200.0, nyquist };

 	SDL_AudioSpec* wavSpec = GetSDL_AudioSpec(packet);

  	int frames = bytesRead / (wavSpec->channels * packet->bitsize / 8);
  	struct FFTW_Results* results = GetFFTW_Results(packet);


  	for(int i=0; i<BUCKETS; ++i) peakmaxArray[i] = 1.7E-308;

	for(int j = 0; j < frames/2; ++j){

		re =  fftwop.out[j][0];
        	im =  fftwop.out[j][1];
      
       	 	magnitude = sqrt(re*re+im*im);
        	//phase = atan(im/re);
        	double freq = j * (double)wavSpec->freq / frames;

        	for (int i = 0; i < BUCKETS; ++i){
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
	results[packet_index].peakfreq[ch] = max_index*(double)wavSpec->freq / frames;
/*
	if(phase > results[packet_index].phase)
		results[packet_index].phase = phase;*/
	for(int i =0; i< BUCKETS; ++i){
		results[packet_index].peakmagMatrix[ch][i] =  10*(log10(peakmaxArray[i]));

	}

	free(peakmaxArray);
}


	