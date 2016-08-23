#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "audioInformation.h"
#include "SDL2/SDL.h"
#include <fftw3.h>


int getFileSize(FILE *inFile);
void analyze_FFTW_Results(Visualizer_Pkg_ptr, struct FFTWop, int, int,size_t);
void setupDFTForStereo(Visualizer_Pkg_ptr, Uint8*, int );
void setupDFTForMono(Visualizer_Pkg_ptr, Uint8*, int );
void processWAVFile(Uint32 , int , Visualizer_Pkg_ptr );

#endif