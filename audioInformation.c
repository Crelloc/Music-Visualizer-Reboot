#include "audioInformation.h"
#include <SDL2/SDL.h>
#include <fftw3.h>

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{

    struct AudioData* audio = (struct AudioData*)userdata;

    if(audio->currentLength == 0)  
        return;
       
    Uint32 length = (Uint32)streamLength;
    length = (length > audio->currentLength ? audio->currentLength : length);
    
    SDL_memcpy(stream, audio->currentPos, length);

    audio->currentPos += length;
    audio->currentLength -= length;
 
   

}