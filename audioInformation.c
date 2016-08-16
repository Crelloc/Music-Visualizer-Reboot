#include "audioInformation.h"
#include <SDL2/SDL.h>
#include <fftw3.h>

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{

    struct AudioData* audio= (struct AudioData*)userdata;
    
    if(audio->currentLength == 0)  
        return;
       
    Uint32 length = (Uint32)streamLength;
    length = (length > audio->currentLength ? audio->currentLength : length);
    
    SDL_memcpy(stream, audio->currentPos, length);

    audio->currentPos += length;
    audio->currentLength -= length;
 
   

}

double Get8bitAudioSample(Uint8** bytebuffer,SDL_AudioFormat format)
{
	
	return ;
}

double Get16bitAudioSample(Uint8** bytebuffer, SDL_AudioFormat format)
{
	Uint16* val =  (Uint16*)malloc(sizeof(val));

	if(SDL_AUDIO_ISLITTLEENDIAN(format))
		*val = (uint16_t)(*bytebuffer)[0] | ((uint16_t)(*bytebuffer)[1] << 8);
		
	
	else
		*val = ((uint16_t)(*bytebuffer)[0] << 8) | (uint16_t)(*bytebuffer)[1];
		 
	

	*bytebuffer+=2;

	return;
}

double Get32bitAudioSample(Uint8** bytebuffer, SDL_AudioFormat format)
{
	return ;
}

