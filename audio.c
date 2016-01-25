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

#define USE_OMP
//#define NO_OF_CORES
//#define DEBUG_TIMING

#ifdef USE_OMP
  #include <omp.h>
#endif

#define SAMPLERATE 44100
#define NUMAUDIOCHANNELS 2
#define NUMCHUNKCHANNELS 48
#define BUFFSIZE 2048
#define MAXFILEINFOTOKENS 2
#define FILENAME 0
#define LOOPLENGTH 1
#define DEFAULTVOLUME 32
#define DEFAULTCHANNEL INT_MAX
#define BEATS_IN_A_BAR 4

//All to aid readability of audio_mainLoop
#define SAMPLE_IS_ACTIVE(i) activeSamples[i].active
#define SET_VOLUME(i) Mix_Volume(activeSamples[i].channel, activeSamples[i].volume)
#define REPEATS_LEFT(i) activeSamples[i].repeatsLeft
#define BARS_LEFT(i) activeSamples[i].barsLeft
#define BARS_IN_LOOP(i) activeSamples[i].loopLength
#define LOOP_IS_NOT_PLAYING(i) !Mix_Playing(activeSamples[i].channel)
#define LOOP_IS_PLAYING(i) Mix_Playing(activeSamples[i].channel)
#define REMOVE_LOOP(i) audio_removeLoop(i)
#define RESTART_LOOP(i) audio_startLoop(i)
#define STOP_LOOP(i) Mix_HaltChannel(activeSamples[i].channel)

//Main Sample Structure
typedef struct {
  Mix_Chunk *sample;
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

/* Debug */
void printTiming(void);

Uint32 newTime, expectedTime;
int32_t time_difference;

void audio_init(void) {
  SDL_Init(SDL_INIT_AUDIO);

  if( Mix_OpenAudio( SAMPLERATE, MIX_DEFAULT_FORMAT, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ) {
  /*if( Mix_OpenAudio( SAMPLERATE, AUDIO_F32SYS, NUMAUDIOCHANNELS, BUFFSIZE ) < 0 ) {*/
    fprintf(stderr, "Audio: ERROR: SDL_mixer: %s\n", Mix_GetError());
  }
  if(Mix_AllocateChannels(NUMCHUNKCHANNELS) != NUMCHUNKCHANNELS){
    fprintf(stderr, "Audio: ERROR: Could Not Allocate Enough Channels: %s\n", Mix_GetError());
  }
  readSampleInfo();
}

//-------------------------------------------------------

/* Loops through activeSamples array once every bar, checks which samples
   have stopped and retriggers them if they have repeats left, otherwise
   removes and frees them */
void audio_mainLoop(void) {
  int i;

  #ifdef DEBUG_TIMING
    printTiming();
  #endif
  #ifdef NO_OF_CORES
  int flag = 0;
  if(!flag){
    flag = 1;
    printf("Threads: %d\n", omp_get_num_threads());
  }
  #endif
  #ifdef USE_OMP
    #pragma omp parallel for
  #endif
  for(i = 0; i < MAXNUMBEROFSAMPLES; i++) {
    if(SAMPLE_IS_ACTIVE(i)) {
      SET_VOLUME(i);
      if((BARS_LEFT(i) == 0) && (REPEATS_LEFT(i) != 0)) { /* Sample needs re-triggering */
        BARS_LEFT(i) = BARS_IN_LOOP(i);
        if(LOOP_IS_PLAYING(i)) {
          STOP_LOOP(i);
        }
        RESTART_LOOP(i);
        if(REPEATS_LEFT(i) > 0) {
          REPEATS_LEFT(i)--;
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

//-------------------------------------------------------

// Add a loop to the activeSamples array to be triggered by the main loop
void audio_addLoop(int index) {
  Sample sample;
  if (activeSamples[index].sample == NULL) {
    printf("AUDIO MADE ACTIVE\n");
    sample = loadSample(index);
    addToActiveArray(index, sample);
    setLoopActiveFlag(index, true);
  }
}

Sample loadSample(int index) {
  Sample sample = {NULL, DEFAULTCHANNEL, false, 1, 0, -1, DEFAULTVOLUME}; //must read in looplength and repeatsleft from the file/user input
  sample.sample = Mix_LoadWAV(sampleFilePaths[index]);
    sample.loopLength = sampleLoopLengths[index];
  if (!sample.sample) {
    fprintf(stderr, "Audio: ERROR Failed to load sample \"%s\"!\n", sampleFilePaths[index]);
  }
  return sample;
}

void addToActiveArray(int index, Sample sample) {
  activeSamples[index] = sample;
}

// Sets the flag that tells the main loop that the sample should be playing
void setLoopActiveFlag(int index, bool flag) {
  activeSamples[index].active = flag;
}

//-------------------------------------------------------

void audio_startLoop(int index) {
  activeSamples[index].channel = Mix_PlayChannel(-1, activeSamples[index].sample, activeSamples[index].repeatsLeft);
  if(activeSamples[index].channel == -1){
    fprintf(stderr, "Audio: ERROR: Could Not Assign Loop To A Channel\n");
  }

  Mix_Volume(activeSamples[index].channel, activeSamples[index].volume);
}

//-------------------------------------------------------

void audio_removeLoop(int index) {
  if(Mix_Playing(activeSamples[index].channel)){
    Mix_HaltChannel(activeSamples[index].channel);
  }
    Mix_FreeChunk(activeSamples[index].sample);
    activeSamples[index].sample = NULL;
    activeSamples[index].channel = DEFAULTCHANNEL;
    setLoopActiveFlag(index, false);
}

//-------------------------------------------------------

// Tells audio main loop that the relevant sample has to be removed
void audio_markLoopInactive(int index) {
  setLoopActiveFlag(index, false);
}

//-------------------------------------------------------

void audio_playSampleOnce(int index) {
  Sample buttonSound = {NULL, DEFAULTCHANNEL, false, 1, 0, 1, DEFAULTVOLUME};
  if(!buttonSound.sample) {
    buttonSound = loadSample(index);
    buttonSound.repeatsLeft = 1;
    addToActiveArray(MAXNUMBEROFSAMPLES-1, buttonSound);
    setLoopActiveFlag(MAXNUMBEROFSAMPLES-1, true);
  }
}

//-------------------------------------------------------

// Stop all samples
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

//-------------------------------------------------------

// Reads the sample file path and the number of bars into
// the relevant arrays in the program
void readSampleInfo() {
  FILE *sampleInfoFile;
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
    if(audio_noOfSamplesLoaded > MAXNUMBEROFSAMPLES) {
      fprintf(stderr, "Too many samples. Maximum number of samples is %d.\n", MAXNUMBEROFSAMPLES);
      exit(1);
    }
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

// Separates the sample file paths and the loop length while it's being read from the file
void tokenizeSampleInfo(char *sampleInfo, char *tokens[]) {
  int i = 0;

  tokens[i++] = strtok(sampleInfo, " ");
  while(i < MAXFILEINFOTOKENS) {
    tokens[i++] = strtok(NULL, " ");
  }
}

//-------------------------------------------------------

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

//-------------------------------------------------------

// Prints the difference between the expected call time of the audio main
// loop, and the actual call time in ms.
// Can be switched on by uncommenting the #define DEBUG_TIMING
void printTiming(void) {
  newTime = SDL_GetTicks();
  if(newTime != expectedTime){
    time_difference = newTime - expectedTime;
    if(time_difference > 0){
      printf("TIMING ERROR. Call to audio_mainLoop was %d ms too late\n", time_difference);
    }
    else{
      printf("TIMING ERROR. Call to audio_mainLoop was %d ms too early\n", time_difference*(-1));
    }
  }
  expectedTime = newTime + (480*4);
}
