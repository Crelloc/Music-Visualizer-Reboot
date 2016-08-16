#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "audioInformation.h"
#include <SDL2/SDL.h>


void processWAVFile(Uint8* wavStart, Uint32 wavLength, int buffer_size, Visualizer_Pkg_ptr vis_pkg_ptr);

#endif