#include <stdio.h>
#include <SDL2/SDL.h>

#ifdef __APPLE__
	#include <SDL2_mixer/SDL_mixer.h>
#elif __LINUX__
	#include <SDL2/SDL_mixer.h>
#endif

#include "audio.h"

#define SAMPLERATE 44100
#define NUMAUDIOCHANNELS 2
#define BUFFSIZE 2048

Mix_Chunk *drum1_sound = NULL;
Mix_Chunk *clap1_sound = NULL;

/*https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_28.html*/

void audio_init(void){
	SDL_Init(SDL_INIT_AUDIO);
  /*                 FS, sample format, num channels, sample size 2kb*/
  if( Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ){
    fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError() );
  }
  drum1_sound = Mix_LoadWAV("samples/FatkickVES2023.wav");
  clap1_sound = Mix_LoadWAV("samples/MUB1Clap004.wav");
  if(drum1_sound == NULL){
    fprintf(stderr, "Drums failed %s\n", Mix_GetError());
  }
  if(clap1_sound == NULL){
    fprintf(stderr, "Clap failed %s\n", Mix_GetError());
  }
}

void audio_close(void){
  Mix_FreeChunk(drum1_sound);
  Mix_FreeChunk(clap1_sound);

  drum1_sound = NULL;

  clap1_sound = NULL;

  Mix_Quit();
	SDL_Quit();
}

void audio_play1(void){
  Mix_PlayChannel( -1, drum1_sound, 0 ); /* (channel -1 = dont care, sound, times to repeat)*/
}

void audio_play2(void){
  Mix_PlayChannel( -1, clap1_sound, 0 );
}
