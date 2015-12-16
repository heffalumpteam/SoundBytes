#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "text.h"
#include "audio.h"

enum instruction{
  ADD,
  REMOVE,
  SET,
  STOP
};
typedef enum instruction Instruction;

void instructionControl(char *string_pointer);
void addLoop(char *string);
void removeLoop(char *string);
void stopAllLoops(char *string);
void setLoop(char *string_pointer);
void setVolume(char *string_pointer, Loop index);
void passBack(char *input_string);
int selectLoop(char *string);
int selectInstruction(char *string);

void text_mainLoop(void)
{
  audio_mainLoop();
}

void text_receiveButtonPress(char *input_string)
{
  int loop_to_play_once = selectLoop(input_string);
  switch(loop_to_play_once){
    case DRUMS_SHUFFLE: audio_playSampleOnce(DRUMS_SHUFFLE); break;
    case DRUMS_CLAP: audio_playSampleOnce(DRUMS_CLAP); break;
    case BASS: audio_playSampleOnce(BASS); break;
    case KEYS: audio_playSampleOnce(KEYS); break;
  }
}

void text_receiveUpdate(char *input_string)
{
  int j;
  int i = 1;
  char* string_pointer;

  for (j= 0; input_string[j] != '\0'; j++) {
     input_string[j] = tolower(input_string[j]);
   }
  /*Takes input line from GUI and splits into individual words*/
  string_pointer = strtok(input_string, " \n.()");

  while(string_pointer){
    printf("TEXT: Instruction %d Received: %s\n", i, string_pointer);
    instructionControl(string_pointer);
  }
  string_pointer = strtok(NULL, " \n.()");
  i++;
}

void instructionControl(char *string_pointer)
{
  char *input_string;
  int instruction_to_execute = selectInstruction(string_pointer);

  switch(instruction_to_execute){
    case ADD:
      string_pointer = strtok(NULL, " \n.()");
      addLoop(string_pointer);
    break;

    case REMOVE:
      string_pointer = strtok(NULL, " \n.()");
      removeLoop(string_pointer);
    break;

    case SET:
      string_pointer = strtok(NULL, " \n.()");
      setLoop(string_pointer);
    break;

    case STOP:
      stopAllLoops(string_pointer);
    break;
  }
    input_string = strtok(NULL, "");
    passBack(input_string);
}

void addLoop(char *string)
{
  int loop_to_add;

  if(string){
    printf("TEXT: Add function: Instrument: %s\n", string);

    loop_to_add = selectLoop(string);
    printf("loop_to_add: %d\n", loop_to_add);

    switch(loop_to_add) {
      case DRUMS_SHUFFLE: audio_addLoop(DRUMS_SHUFFLE); break;
      case DRUMS_CLAP: audio_addLoop(DRUMS_CLAP); break;
      case BASS: audio_addLoop(BASS); break;
      case KEYS: audio_addLoop(KEYS); break;
    }
  }
}

void removeLoop(char *string)
{
  int loop_to_remove;

  if(string){
    printf("TEXT: Remove function: Instrument: %s\n", string);

    loop_to_remove = selectLoop(string);
    printf("loop_to_remove: %d\n", loop_to_remove);

    switch(loop_to_remove) {
      case DRUMS_SHUFFLE: audio_markLoopInactive(DRUMS_SHUFFLE); break;
      case DRUMS_CLAP: audio_markLoopInactive(DRUMS_CLAP); break;
      case BASS: audio_markLoopInactive(BASS); break;
      case KEYS: audio_markLoopInactive(KEYS); break;
    }
  }
}

void setLoop(char *string_pointer)
{
  int set_loop_volume;
  char *temp;
  if(string_pointer){
    // temp now points to "volume".
    temp = strtok(NULL, " \n.()");

    if (strcmp(temp, "volume") == 0){
      set_loop_volume = selectLoop(string_pointer);

      //switch only compares earlier pointer - which hopefully
      //points to the instrument
      switch(set_loop_volume){
        case DRUMS_SHUFFLE: setVolume(string_pointer, DRUMS_SHUFFLE); break;
        case DRUMS_CLAP: setVolume(string_pointer, DRUMS_CLAP); break;
        case BASS: setVolume(string_pointer, BASS); break;
        case KEYS: setVolume(string_pointer, KEYS); break;
      }
    }
  }
}


void setVolume(char *string_pointer, Loop index)
{
  int i, volume;
  char *input_string;
  string_pointer = strtok(NULL, " \n.()");
  // If the string contains a non-numerical values pass back
  for (i = 0; (string_pointer[i] != '\0'); i++) {
    if (isdigit(string_pointer[i]) == 0) {
      printf("ERROR: That wasn't a valid number!\n");
      input_string = strtok(NULL, "");
      passBack(input_string);
    }
  }
  // Convert the string to an int
  volume = atoi(string_pointer);

  // The user entered a valid volume number
  if ((volume > 0) && (volume < 12)) {
    printf("atoi check: %d\n", volume);
    volume = ((128 / 11) * volume);
    audio_changeVolume(index, volume);
  }
  // Invalid no
  else {
    printf("Next time please enter a number between 0 and 11!\n");
    input_string = strtok(NULL, "");
    passBack(input_string);
  }
}

void stopAllLoops(char *string)
{
  if(string){
    audio_stop();
    printf("TEXT: All loops stopped.\n");
  }
}

void passBack(char *input_string)
{
  if(input_string){
    printf("PASSED BACK %s\n", input_string);
    text_receiveUpdate(input_string);
  }
}

int selectLoop(char *string)
{
  if(strcmp(string, "drum") == 0){
     printf("TEXT: drum\n");
     return DRUMS_SHUFFLE;
  }
  if(strcmp(string, "clap") == 0){
     printf("TEXT: clap\n");
     return DRUMS_CLAP;
  }
  if(strcmp(string, "bass") == 0){
     printf("TEXT: bass\n");
     return BASS;
  }
  if(strcmp(string, "keys") == 0){
     printf("TEXT: keys\n");
     return KEYS;
  }
  return -1;
}

int selectInstruction(char *string)
{
  if(strcmp(string, "add") == 0){
    printf("TEXT: add\n");
    return ADD;
  }
  if(strcmp(string, "remove") == 0){
    printf("TEXT: remove\n");
    return REMOVE;
  }
  if(strcmp(string, "set") == 0){
    printf("TEXT: set\n");
    return SET;
  }
  if(strcmp(string, "stop") == 0){
    printf("TEXT: stop\n");
    return STOP;
  }
  return -1;
}
