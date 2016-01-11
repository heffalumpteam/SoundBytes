#ifndef _audio
#define _audio

enum instrument{
  DRUM,
  BASS,
  KEYS
};
typedef enum instrument Instrument;

enum loop{
  DRUM_KICK,
  DRUM_CLAP,
  BASS_1,
  KEYS_1
};
typedef enum loop Loop;

int audio_noOfSamplesLoaded;

void audio_init(void);
void audio_close(void);
void audio_mainLoop(void);
void audio_playSampleOnce(Loop index);
void audio_startLoop(Loop index);
void audio_stopLoop(Loop index);
void audio_addLoop(Loop index);
void audio_removeLoop(Loop index);
void audio_markLoopInactive(Loop index);
void audio_stop(void);
void audio_changeVolume(Loop index, int volume);

#endif
