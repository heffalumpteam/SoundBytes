#ifndef AUDIO_H_
#define AUDIO_H_

int audio_noOfSamplesLoaded;

void audio_init(void);
void audio_close(void);
void audio_mainLoop(void);
void audio_playSampleOnce(int index);
void audio_startLoop(int index);
void audio_stopLoop(int index);
void audio_addLoop(int index);
void audio_removeLoop(int index);
void audio_markLoopInactive(int index);
void audio_stop(void);
void audio_changeVolume(int index, int volume);

gpointer sdl_thread(gpointer data);

#endif
