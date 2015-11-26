enum loop{
  DRUMS_SHUFFLE,
  DRUMS_CLAP,
  SYNTH_STAB
};
typedef enum loop Loop;

enum instruction{
  START, /*Starting playback*/
  STOP,  /*Stop playback*/
  ADD,   /*Add a loop*/
  REMOVE /*Stop a loop*/
};
typedef enum instruction Instruction;

void audio_init(void);
void audio_close(void);
void audio_mainLoop(void);
void audio_playSampleOnce(Loop index);
void audio_startLoop(Loop index);
void audio_stopLoop(Loop index);
void audio_addLoop(Loop index);
void audio_removeLoop(Loop index);
void audio_markLoopInactive(Loop index);
