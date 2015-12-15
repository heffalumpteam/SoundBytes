#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "text.h"
#include "audio.h"

int loopSelection(char *string);
void addRemoveStopLoop(char *string_pointer);
void setFunction(char *string_pointer);
void setVolume(char *string_pointer, Loop index);
void passBack(char *input_string);

// SEGFAULTS WHEN RUNNING COMMANDS ON LOOPS OTHER THAN KEYS

void text_mainLoop(void)
{
  audio_mainLoop();
}

int loopSelection(char *string)
{
  if(strcmp(string, "drums") == 0){
     printf("TEXT: drums\n");
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

void text_receiveButtonPress(char *input_string)
{
  int loop_to_play_once = loopSelection(input_string);
  switch(loop_to_play_once){
    case DRUMS_SHUFFLE: audio_playSampleOnce(DRUMS_SHUFFLE); break;
    case DRUMS_CLAP: audio_playSampleOnce(DRUMS_CLAP); break;
    case BASS: audio_playSampleOnce(BASS); break;
    case KEYS: audio_playSampleOnce(KEYS); break;
  }
}

void text_receiveUpdate(char *input_string)
{
  char* string_pointer;
  int j;
  int i = 1;

  for (j= 0; input_string[j] != '\0'; j++) {
     input_string[j] = tolower(input_string[j]);
   }
/*Takes input line from GUI and splits into individual words*/
  string_pointer = strtok(input_string, " \n.()");
  while(string_pointer){
    printf("TEXT: Instruction %d Received: %s\n", i, string_pointer);
    if((strcmp(string_pointer, "add") == 0) || (strcmp(string_pointer, "remove") == 0) || \
       (strcmp(string_pointer, "stop") == 0)){
      addRemoveStopLoop(string_pointer);
    }
    if(strcmp(string_pointer, "set") == 0) {
      setFunction(string_pointer);
    }
    string_pointer = strtok(NULL, " \n.()");
    i++;
  }
}

void addRemoveStopLoop(char *string_pointer)
{
  int loop_to_add, loop_to_remove;
  char *input_string;

  if(strcmp(string_pointer, "add") == 0){
    string_pointer = strtok(NULL, " \n.()");

    if(string_pointer){
      printf("TEXT: Add function: Instrument: %s\n", string_pointer);
      loop_to_add = loopSelection(string_pointer);
      switch(loop_to_add) {
        case DRUMS_SHUFFLE: audio_addLoop(DRUMS_SHUFFLE); break;
        case DRUMS_CLAP: audio_addLoop(DRUMS_CLAP); break;
        case BASS: audio_addLoop(BASS); break;
        case KEYS: audio_addLoop(KEYS); break;
      }
    }
  }
  if (strcmp(string_pointer, "remove") == 0){
    string_pointer = strtok(NULL, " \n.()");

    if(string_pointer){
      printf("TEXT: Remove function: Instrument: %s\n", string_pointer);
      loop_to_remove = loopSelection(string_pointer);
      switch(loop_to_remove) {
        case DRUMS_SHUFFLE: audio_markLoopInactive(DRUMS_SHUFFLE); break;
        case DRUMS_CLAP: audio_markLoopInactive(DRUMS_CLAP); break;
        case BASS: audio_markLoopInactive(BASS); break;
        case KEYS: audio_markLoopInactive(KEYS); break;
      }
    }
  }
  if (strcmp(string_pointer, "stop") == 0){
    if(string_pointer){
      audio_stop();
      printf("TEXT: All loops stopped.\n");
    }
  }
//If there are other intructions (on other lines) to carry out, send
//them back to text_recievUpdate function to sort out
    input_string = strtok(NULL, "");
    passBack(input_string);
}

void setFunction(char *string_pointer)
{
  int set_loop_volume;
  char *input_string;

  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    printf("Set function: Instrument: %s\n", string_pointer);
    set_loop_volume = loopSelection(string_pointer);
    switch(set_loop_volume) {
      case DRUMS_SHUFFLE:
        string_pointer = strtok(NULL, " \n.()");
        if(strcmp(string_pointer, "volume") == 0){
          setVolume(string_pointer, DRUMS_SHUFFLE);
        }
      case DRUMS_CLAP:
        string_pointer = strtok(NULL, " \n.()");
        if(strcmp(string_pointer, "volume") == 0){
          setVolume(string_pointer, DRUMS_CLAP);
        }
      case BASS:
        string_pointer = strtok(NULL, " \n.()");
        if(strcmp(string_pointer, "volume") == 0){
          setVolume(string_pointer, BASS);
        }
      case KEYS:
        string_pointer = strtok(NULL, " \n.()");
        if(strcmp(string_pointer, "volume") == 0){
          setVolume(string_pointer, KEYS);
        }
    }
  }
  input_string = strtok(NULL, "");
  passBack(input_string);
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

void passBack(char *input_string)
{
  if(input_string){
    printf("PASSED BACK %s\n", input_string);
    text_receiveUpdate(input_string);
  }
}
