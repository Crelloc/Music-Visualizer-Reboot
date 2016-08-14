#include "audioInformation.h"
#include <SDL2/SDL.h>
#include <fftw3.h>

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{

    struct Visualizer_Pkg* vis_pkg = (struct Visualizer_Pkg*)userdata;
    struct AudioData* audio = &vis_pkg->AudioData_t;

    if(audio->currentLength == 0)  
        return;
       
    Uint32 length = (Uint32)streamLength;
    length = (length > audio->currentLength ? audio->currentLength : length);
    
    SDL_memcpy(stream, audio->currentPos, length);

    audio->currentPos += length;
    audio->currentLength -= length;
 
   

}
