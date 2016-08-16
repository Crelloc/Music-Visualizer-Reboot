#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "audioInformation.h"
#include <SDL2/SDL.h>
#include <fftw3.h>

void processWAVFile(Uint32 wavLength, int buffer_size, Visualizer_Pkg_ptr vis_pkg_ptr);

#endif