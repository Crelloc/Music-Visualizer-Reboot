#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "audioInformation.h"
#include "dataprocessing.h"


#define FILE_PATH "/home/crelloc/Music/Em-Infinite.wav"


static volatile int keeprunning = 1;

void aborted(int sig){

  printf("\nAborted by signal: %d\n", sig);
  keeprunning = 0;
}

int main(int argc, char** argv)
{

  (void) signal(SIGINT, aborted);
  (void) signal(SIGTSTP, aborted);

  SDL_Init(SDL_INIT_AUDIO);

  SDL_AudioSpec wavSpec;
  Uint8* wavStart;
  Uint32 wavLength;

  if(SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL)
  {
    // TODO: Proper error handling
  
    return 1;
  }


  struct AudioData AudioData_t;
  AudioData_t.currentPos = wavStart;
  AudioData_t.wavStart = wavStart;
  AudioData_t.wavLength = wavLength;
  AudioData_t.currentLength = wavLength;
  wavSpec.callback = MyAudioCallback;
  wavSpec.userdata = &AudioData_t;

  
  
  SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL,
      SDL_AUDIO_ALLOW_ANY_CHANGE);
  if(device == 0)
  {
    // TODO: Proper error handling
    
    return 1;
  }
  double (*function_ptr)(Uint8* bytebuffer, SDL_AudioFormat format);
  int whatformat = (int)SDL_AUDIO_BITSIZE(wavSpec.format);

  switch(whatformat){
    case 8: 
        printf("8 bit data samples\n");
        function_ptr = Get8bitAudioSample;
        break;
    case 16:
        printf("16 bit data samples\n");
        function_ptr = Get16bitAudioSample;
        break;
    case 32:
        function_ptr = Get32bitAudioSample;
        printf("32 bit data samples\n");
        break;
    default:
        break;

  }
  
  struct Visualizer_Pkg visualizer_pkg_t = {
                                            .filename = FILE_PATH,
                                            .packetIndex = 0,
                                            .device = device,
                                            .AudioData_ptr = &AudioData_t,
                                            .wavSpec_ptr = &wavSpec,
                                            .FFTW_Results_ptr = NULL,
                                            .GetAudioSample = function_ptr
                                          };
  

  processWAVFile(wavStart, wavLength, &visualizer_pkg_t);

  SDL_PauseAudioDevice(device, 0);

  while(AudioData_t.currentLength > 0 && keeprunning)
  {
    
    SDL_Delay(100);
  }

  SDL_CloseAudioDevice(device);
  SDL_FreeWAV(wavStart);
  SDL_Quit();
  return 0;
}
