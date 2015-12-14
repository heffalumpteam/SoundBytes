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
#define DEFAULTCHANNEL INT_MAX
#define BEATS_IN_A_BAR 4

#define SAMPLE_IS_ACTIVE(i) activeSamples[i].active
#define REPEATS_LEFT(i) activeSamples[i].repeatsLeft
#define BARS_LEFT(i) activeSamples[i].barsLeft
#define BARS_IN_LOOP(i) activeSamples[i].loopLength
#define	LOOP_IS_NOT_PLAYING(i) !Mix_Playing(activeSamples[i].channel)
#define REMOVE_LOOP(i) audio_removeLoop(i)
#define RESTART_LOOP(i) audio_startLoop(i)

struct sample
{
  Mix_Chunk* sample;
  int channel;
  bool active;
	int loopLength;
	int barsLeft;
	int repeatsLeft; //-1 to keep playing indefinitely
};
typedef struct sample Sample;

Sample loadSample(Loop index);
void populateFilePathsArray(char* sampleFilePaths[]);
void addToActiveArray(Loop index, Sample sample);
void setLoopActiveFlag(Loop index, bool flag);

/* Will hold all the paths to the samples for easy reference during runtime. Thoughts? */
char* sampleFilePaths[MAXNUMBEROFSAMPLES];
Sample activeSamples[MAXNUMBEROFSAMPLES] = {{NULL, DEFAULTCHANNEL, false, 1, 0, -1}};
int channel1, channel2, abeat = 0, abar = 0;

Sample buttonSound;
Mix_Chunk *clap1_sound = NULL;

/*https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_28.html*/

void audio_init(void){
  SDL_Init(SDL_INIT_AUDIO);
	if( Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ){
    fprintf(stderr, "Audio: SDL_mixer Error: %s\n", Mix_GetError());
  }
  populateFilePathsArray(sampleFilePaths);
}

void audio_mainLoop(void){
	int i;
	for(i = 0; i < MAXNUMBEROFSAMPLES; i++) {
		if(SAMPLE_IS_ACTIVE(i)){
			if(REPEATS_LEFT(i) > 0){
				REPEATS_LEFT(i)--;
			}
			if((BARS_LEFT(i) == 0) && (REPEATS_LEFT(i) != 0)){
				BARS_LEFT(i) = BARS_IN_LOOP(i);
				if(LOOP_IS_NOT_PLAYING(i)){
			    RESTART_LOOP(i);
				}
		  }
			else if(REPEATS_LEFT(i) == 0){
				REMOVE_LOOP(i);
			}
		  BARS_LEFT(i)--;
		}
		else{
			if(activeSamples[i].sample != NULL){
				REMOVE_LOOP(i);
			}
		}
	}

	/*if there is sound loaded by pressing a button and it's not playing*/
	if( (buttonSound.sample) && (!Mix_Playing(buttonSound.channel)) ){
			Mix_FreeChunk(buttonSound.sample);
			buttonSound.sample = NULL;
			buttonSound.channel = DEFAULTCHANNEL;
	}
}

void audio_addLoop(Loop index)
{
	Sample sample;
  if (activeSamples[index].sample == NULL){
		printf("AUDIO MADE ACTIVE\n");
    sample = loadSample(index);
    addToActiveArray(index, sample);
    setLoopActiveFlag(index, true);
  }
}

void setLoopActiveFlag(Loop index, bool flag)
{
  activeSamples[index].active = flag;
}

Sample loadSample(Loop index) {
//for some reason setting channel to 0 doesn't work
  Sample sample = {NULL, DEFAULTCHANNEL, false, 1, 0, -1}; //must read in looplength and repeatsleft from the file/user input
  sample.sample = Mix_LoadWAV(sampleFilePaths[index]);
  if (!sample.sample){
    fprintf(stderr, "Audio: Failed to load sample \"%s\"!\n", sampleFilePaths[index]);
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

void audio_startLoop(Loop index) {
   activeSamples[index].channel = Mix_PlayChannel(-1, activeSamples[index].sample, activeSamples[index].repeatsLeft);
	   /* (channel -1 = dont care, sound, times to repeat)*/
}

void audio_removeLoop(Loop index) {
  if(Mix_Playing(activeSamples[index].channel)){
    Mix_HaltChannel(activeSamples[index].channel);
    Mix_FreeChunk(activeSamples[index].sample);
    activeSamples[index].sample = NULL;
    activeSamples[index].channel = DEFAULTCHANNEL;
  }
	else{
		Mix_FreeChunk(activeSamples[index].sample);
    activeSamples[index].sample = NULL;
    activeSamples[index].channel = DEFAULTCHANNEL;
	}
}

void audio_markLoopInactive(Loop index)
{
  printf("Audio: inactive\n");
  setLoopActiveFlag(index, false);
}

void audio_playSampleOnce(Loop index)
{
	if(!buttonSound.sample) {
		buttonSound = loadSample(index);
		buttonSound.channel = Mix_PlayChannel(-1, buttonSound.sample, 0);
	}
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
