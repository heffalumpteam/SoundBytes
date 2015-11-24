#include <stdio.h>
#include <SDL2/SDL.h>
#ifdef __APPLE__
	#include <SDL2_mixer/SDL_mixer.h>
#elif __LINUX__
	#include <SDL2/SDL_mixer.h>
#elif _WIN32
  #include <SDL2/SDL_mixer.h>
#elif _WIN64
  #include <SDL_mixer.h>
#endif

#include "audio.h"
#include <string.h>

#define SAMPLERATE 44100
#define NUMAUDIOCHANNELS 2
#define BUFFSIZE 2048
#define MAXNUMBEROFSAMPLES 50
#define MAXSAMPLENAMELENGTH 30

Mix_Chunk* loadSample(char* filename);
void populateFilePathsArray();
int startSample(Mix_Chunk* sampleToPlay);

/* Will hold all the paths to the samples for easy reference during runtime. Thoughts? */
char* sampleFilePaths[MAXNUMBEROFSAMPLES];
int channel1, channel2;

Mix_Chunk *drum1_sound = NULL;
Mix_Chunk *clap1_sound = NULL;

/*https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_28.html*/

void audio_init(void){
	SDL_Init(SDL_INIT_AUDIO);
  /*                 FS, sample format, num channels, sample size 2kb*/
  if( Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ){
    fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
  }

  populateFilePathsArray(sampleFilePaths);

  drum1_sound = loadSample(sampleFilePaths[0]);
  clap1_sound = loadSample(sampleFilePaths[1]);

  // channel1 = audio_startSample(drum1_sound); /* uncomment to test samples */
  // channel2 = audio_startSample(clap1_sound);

  // printf("Drum channel: %d\n, Clap channel: %d\n", channel1, channel2);
}

void audio_close(void){
  Mix_FreeChunk(drum1_sound);
  Mix_FreeChunk(clap1_sound);

  drum1_sound = NULL;
  clap1_sound = NULL;

  Mix_Quit();
	SDL_Quit();
} 

Mix_Chunk* loadSample(char* filename) {

  Mix_Chunk* sample = NULL;
  sample = Mix_LoadWAV(filename);
  if (!sample)
  {
      fprintf(stderr, "Failed to load sample \"%s\"!\n", filename);
  }
  return sample;
}

/* Returns channel that the sample is playing on - need to store
 * and use later for stopping it, fading it, etc. */
int startSample(Mix_Chunk* sampleToPlay) {

  int channel = 0;
  channel = Mix_PlayChannel(-1, sampleToPlay, -1);    /* (channel -1 = dont care, sound, times to repeat)*/

  return channel;
}

void stopSample(int channelNumber, Mix_Chunk** sampleToStop) {

  if (Mix_Playing(channelNumber)) 
  {
    Mix_HaltChannel(channelNumber);
    Mix_FreeChunk(*sampleToStop);
    *sampleToStop = NULL;
  }

}

void audio_playSampleOnce(char* sampleName)
{
  //printf("%s\n", sampleName);
  if (strcmp(sampleName, "clap(shuffle)") == 0) /* Need a neater way to decide which button has been pressed */
  {
    Mix_PlayChannel(-1, clap1_sound, 0);
    //printf("hooray\n");
  }
  if (strcmp(sampleName, "drum(shuffle)") == 0)
  {
    Mix_PlayChannel(-1, drum1_sound, 0);
  }
}

void populateFilePathsArray(char* sampleFilePaths[])
{
  char samplePath[MAXSAMPLENAMELENGTH];
  FILE* sampleNamesFile;
  int ctr = 0;

  sampleNamesFile = fopen("sampleFilePaths.txt", "r");

  while(fgets(samplePath, MAXSAMPLENAMELENGTH, sampleNamesFile) != NULL)
  {
    if (samplePath[strlen(samplePath) - 1] == '\n')
    {
      samplePath[strlen(samplePath) - 1] = '\0';
    }
  
    sampleFilePaths[ctr] = malloc(MAXSAMPLENAMELENGTH);
    strcpy(sampleFilePaths[ctr], samplePath);
    ctr++;
  }
}
