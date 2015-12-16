#ifndef _audio
#define _audio

enum loop{
  DRUMS_SHUFFLE,
  DRUMS_CLAP,
  BASS,
  KEYS
};
typedef enum loop Loop;

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
