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
#include "samples.h"

#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define SAMPLERATE 44100
#define NUMAUDIOCHANNELS 2
#define BUFFSIZE 2048
#define MAXFILEINFOTOKENS 2
#define FILENAME 0
#define LOOPLENGTH 1
#define DEFAULTVOLUME 32
#define DEFAULTCHANNEL INT_MAX
#define BEATS_IN_A_BAR 4

#define SAMPLE_IS_ACTIVE(i) activeSamples[i].active
#define SET_VOLUME(i) Mix_Volume(activeSamples[i].channel, activeSamples[i].volume)
#define REPEATS_LEFT(i) activeSamples[i].repeatsLeft
#define BARS_LEFT(i) activeSamples[i].barsLeft
#define BARS_IN_LOOP(i) activeSamples[i].loopLength
#define LOOP_IS_NOT_PLAYING(i) !Mix_Playing(activeSamples[i].channel)
#define REMOVE_LOOP(i) audio_removeLoop(i)
#define RESTART_LOOP(i) audio_startLoop(i)

typedef struct {
  Mix_Chunk* sample;
  int channel;
  bool active;
  int loopLength;
  int barsLeft;
  int repeatsLeft; //-1 to keep playing indefinitely
  int volume;
} Sample;

int sampleLoopLengths[MAXNUMBEROFSAMPLES];
Sample activeSamples[MAXNUMBEROFSAMPLES] = {{NULL, DEFAULTCHANNEL, false, 1, 0,-1, DEFAULTVOLUME}};

/* Sample functions */
Sample loadSample(int index);
void addToActiveArray(int index, Sample sample);
void setLoopActiveFlag(int index, bool flag);
/* File info */
void readSampleInfo();
void tokenizeSampleInfo(char *sampleInfo, char *tokens[]);
char *createSampleFilePath(char *path);

void audio_init(void) {
  SDL_Init(SDL_INIT_AUDIO);

  if( Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ) {
    fprintf(stderr, "Audio: SDL_mixer Error: %s\n", Mix_GetError());
  }

  readSampleInfo();
}

/* Loops through activeSamples array once every bar, checks which samples have stopped and retriggers them if they have repeats left, otherwise
  removes and frees them */
void audio_mainLoop(void) {
  int i;
  for(i = 0; i < MAXNUMBEROFSAMPLES; i++) {
    if(SAMPLE_IS_ACTIVE(i)) {
      SET_VOLUME(i);
      if((BARS_LEFT(i) == 0) && (REPEATS_LEFT(i) != 0)) { /* Sample needs re-triggering */
        BARS_LEFT(i) = BARS_IN_LOOP(i);
        if(LOOP_IS_NOT_PLAYING(i)) {
          RESTART_LOOP(i);
          if(REPEATS_LEFT(i) > 0) {
            REPEATS_LEFT(i)--;
          }
        }   
      }
      else if(BARS_LEFT(i) == 0 && REPEATS_LEFT(i) == 0) { /* Sample has finished, needs to be removed */
        REMOVE_LOOP(i);
      }
      BARS_LEFT(i)--;
    }
    else {  /* Inactive samples, need to be removed */
      if(activeSamples[i].sample != NULL) {
        REMOVE_LOOP(i);
      }
    }
  }
}

void audio_addLoop(int index) {
  Sample sample;
  if (activeSamples[index].sample == NULL) {
    printf("AUDIO MADE ACTIVE\n");
    sample = loadSample(index);
    addToActiveArray(index, sample);
    setLoopActiveFlag(index, true);
  }
}

void setLoopActiveFlag(int index, bool flag) {
  activeSamples[index].active = flag;
}

Sample loadSample(int index) {
  Sample sample = {NULL, DEFAULTCHANNEL, false, 1, 0, -1, DEFAULTVOLUME}; //must read in looplength and repeatsleft from the file/user input
  sample.sample = Mix_LoadWAV(sampleFilePaths[index]);
    sample.loopLength = sampleLoopLengths[index];
  if (!sample.sample) {
    fprintf(stderr, "Audio: Failed to load sample \"%s\"!\n", sampleFilePaths[index]);
  }
  return sample;
}

void addToActiveArray(int index, Sample sample) {
  activeSamples[index] = sample;
}

void audio_close(void) {
  int i;

  for (i = 0; i < MAXNUMBEROFSAMPLES; ++i) {
    audio_removeLoop(i);
      if(sampleFilePaths[i]) {
        free(sampleFilePaths[i]);
      }
  }

  Mix_Quit();
  SDL_Quit();
}

void audio_startLoop(int index) {
  activeSamples[index].channel = Mix_PlayChannel(-1, activeSamples[index].sample, activeSamples[index].repeatsLeft);
    /* have to initialise volume here because channel is not set until this point.
    After this audio_mainLoop takes over and checks the volume at regular intervals */
  Mix_Volume(activeSamples[index].channel, activeSamples[index].volume);
}

void audio_removeLoop(int index) {
  if(Mix_Playing(activeSamples[index].channel)){
    Mix_HaltChannel(activeSamples[index].channel);
  }
    Mix_FreeChunk(activeSamples[index].sample);
    activeSamples[index].sample = NULL;
    activeSamples[index].channel = DEFAULTCHANNEL;
    setLoopActiveFlag(index, false); 
}

void audio_markLoopInactive(int index) {
  printf("Audio: inactive\n");
  setLoopActiveFlag(index, false);
}

void audio_playSampleOnce(int index) {
  Sample buttonSound = {NULL, DEFAULTCHANNEL, false, 1, 0, 1, DEFAULTVOLUME};
  if(!buttonSound.sample) {
    buttonSound = loadSample(index);
    buttonSound.repeatsLeft = 1;
    addToActiveArray(MAXNUMBEROFSAMPLES-1, buttonSound);
    setLoopActiveFlag(MAXNUMBEROFSAMPLES-1, true);
  }
}

void audio_stop(void) {
  int i;

  for(i = 0; i < MAXNUMBEROFSAMPLES; i++) {
    if(activeSamples[i].sample != NULL) {
      activeSamples[i].active = false;
    }
  }
}

void audio_changeVolume(int index, int volume) {
  activeSamples[index].volume = volume;
}

void readSampleInfo() {
  FILE* sampleInfoFile;
  char sampleInfo[MAXSAMPLEINFOLENGTH];
  char *tokens[MAXFILEINFOTOKENS];
  int i = 0;

  sampleInfoFile = fopen("sampleFileInfo.txt", "r");
  if(!sampleInfoFile) {
      fprintf(stderr, "Could not open sample information file.\n");
  }

  audio_noOfSamplesLoaded = 0;

  while(fgets(sampleInfo, MAXSAMPLEINFOLENGTH, sampleInfoFile) != NULL) {
    tokenizeSampleInfo(sampleInfo, tokens);
    sampleFilePaths[i] = createSampleFilePath(tokens[FILENAME]);
    sampleLoopLengths[i] = atoi(tokens[LOOPLENGTH]);
    i++;
    audio_noOfSamplesLoaded++;
  }

  fclose(sampleInfoFile);
}

char *createSampleFilePath(char *path) {
  char *newSampleFilePath = malloc(MAXSAMPLEINFOLENGTH);
  if(!newSampleFilePath) {
      fprintf(stderr, "Could not allocate sample file path.\n");
  }
  strcpy(newSampleFilePath, path);
  return newSampleFilePath;
}

void tokenizeSampleInfo(char *sampleInfo, char *tokens[]) {
  int i = 0;

  tokens[i++] = strtok(sampleInfo, " ");
  while(i < MAXFILEINFOTOKENS) {
    tokens[i++] = strtok(NULL, " ");
  }
}
