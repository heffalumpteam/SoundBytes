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
#include <stdbool.h>
#include <limits.h>

#define SAMPLERATE 44100
#define NUMAUDIOCHANNELS 2
#define BUFFSIZE 2048
#define MAXNUMBEROFSAMPLES 50
#define MAXSAMPLENAMELENGTH 30
#define DEFAULTVOLUME 64
#define DEFAULTCHANNEL INT_MAX

struct sample
{
  Mix_Chunk* sample;
  int channel;
  bool active;
};
typedef struct sample Sample;

Sample loadSample(Loop index);
void populateFilePathsArray(char* sampleFilePaths[]);
void addToActiveArray(Loop index, Sample sample);
void setLoopActiveFlag(Loop index, bool flag);

/* Will hold all the paths to the samples for easy reference during runtime. Thoughts? */
char* sampleFilePaths[MAXNUMBEROFSAMPLES];
Sample activeSamples[MAXNUMBEROFSAMPLES] = {{NULL, DEFAULTCHANNEL, false}};
int channel1, channel2;

Sample drum1_sound;
Mix_Chunk *clap1_sound = NULL;

/*https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_28.html*/

  void audio_init(void){
   SDL_Init(SDL_INIT_AUDIO);
  /*                 FS, sample format, num channels, sample size 2kb*/
   if( Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ){
    fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
  }

  populateFilePathsArray(sampleFilePaths);
}

void audio_mainLoop(void)
{
	int i;
	for(i = 0; i < MAXNUMBEROFSAMPLES; i++) {
		if(activeSamples[i].active) {
			audio_playSampleOnce(i);
		}
	}
}

void audio_addLoop(Loop index)
{
  drum1_sound = loadSample(index);
  addToActiveArray(index, drum1_sound);
  setLoopActiveFlag(index, true);
  audio_changeVolume(index, DEFAULTVOLUME);
}

void setLoopActiveFlag(Loop index, bool flag)
{
  activeSamples[index].active = flag;
}

Sample loadSample(Loop index) {

  Sample sample = {NULL, 0, false};

  sample.sample = Mix_LoadWAV(sampleFilePaths[index]);
  if (!sample.sample)
  {
    fprintf(stderr, "Failed to load sample \"%s\"!\n", sampleFilePaths[index]);
  }
  return sample;
}

void addToActiveArray(Loop index, Sample sample)
{
  activeSamples[index] = sample;
}

void audio_close(void){
  int i;

  for (i = 0; i < MAXNUMBEROFSAMPLES; ++i) {
    audio_removeLoop(i);
  }

  Mix_Quit();
  SDL_Quit();
}


// void audio_startLoop(Loop index) {

//   int channel = 0;
//   channel = Mix_PlayChannel(-1, activeSamples[index].sample, -1);
//   activeSamples[index].channel = channel;    /* (channel -1 = dont care, sound, times to repeat)*/
// }

void audio_removeLoop(Loop index) {

  if (Mix_Playing(activeSamples[index].channel))
  {
    Mix_HaltChannel(activeSamples[index].channel);
    Mix_FreeChunk(activeSamples[index].sample);
    activeSamples[index].sample = NULL;
    activeSamples[index].channel = DEFAULTCHANNEL;
  }
}

void audio_markLoopInactive(Loop index)
{
  setLoopActiveFlag(index, false);
}

void audio_playSampleOnce(Loop index)
{
  Mix_PlayChannel(-1, activeSamples[index].sample, 0);
}

//New function added by ADK on 29/11

void audio_stop(void)
{
  int i;
	for(i = 0; i < MAXNUMBEROFSAMPLES; i++) {
    if(activeSamples[i].sample != NULL) {
      activeSamples[i].active = false;
		}
	}
}

void audio_changeVolume(Loop index, int volume)
{
  Mix_Volume(activeSamples[index].channel, volume);
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
