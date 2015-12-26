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
void add_(char *string_pointer);
void addDrum(char *string_pointer);
void addBass(char *string_pointer);
void addKeys(char *string_pointer);
void remove_(char *string_pointer);
void removeDrum(char *string_pointer);
void removeBass(char *string_pointer);
void removeKeys(char *string_pointer);
void set_(char *string_pointer);
void setDrumVolume(char *string_pointer);
void setBassVolume(char *string_pointer);
void setKeysVolume(char *string_pointer);
void changeVolume(char *string_pointer, Loop index);
void stopAll_(char *string_pointer);
void passBack(char *input_string);
int selectInstruction(char *string);
int selectInstrument(char *string);
int selectDrumLoop(char *string);
int selectBassLoop(char *string);
int selectKeysLoop(char *string);
char *createStringCopy(char *input_string);

void text_mainLoop(void)
{
  audio_mainLoop();
}

void text_receiveButtonPress(char *input_string)
{
  int instrument_to_play_once, loop_to_play_once;
  char *string_pointer, *temp;

  temp = string_pointer = createStringCopy(input_string);
  string_pointer = strtok(string_pointer, " \n.()");
  instrument_to_play_once = selectInstrument(string_pointer);
  string_pointer = strtok(NULL, " \n.()");

  switch(instrument_to_play_once){
    case DRUM: loop_to_play_once = selectDrumLoop(string_pointer); break;
    case BASS: loop_to_play_once = selectBassLoop(string_pointer); break;
    case KEYS: loop_to_play_once = selectKeysLoop(string_pointer); break;
  }
  switch(loop_to_play_once){
    case DRUM_KICK: audio_playSampleOnce(DRUM_KICK); break;
    case DRUM_CLAP: audio_playSampleOnce(DRUM_CLAP); break;
    case BASS_1: audio_playSampleOnce(BASS_1); break;
    case KEYS_1: audio_playSampleOnce(KEYS_1); break;
  }
  free(temp);
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
    string_pointer = strtok(NULL, " \n.()");
    i++;
  }
}

void instructionControl(char *string_pointer)
{
  char *input_string;
  int instruction_to_execute = selectInstruction(string_pointer);

  switch(instruction_to_execute){
    case ADD: add_(string_pointer); break;
    case REMOVE: remove_(string_pointer); break;
    case SET: set_(string_pointer); break;
    case STOP: stopAll_(string_pointer); break;
  }
  input_string = strtok(NULL, "");
  passBack(input_string);
}

void add_(char *string_pointer)
{
  int instrument_to_add;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    printf("TEXT: Add function: Instrument: %s\n", string_pointer);

    instrument_to_add = selectInstrument(string_pointer);
    printf("instrument_to_add : %d\n", instrument_to_add );

    switch(instrument_to_add) {
      case DRUM: addDrum(string_pointer); break;
      case BASS: addBass(string_pointer); break;
      case KEYS: addKeys(string_pointer); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void addDrum(char *string_pointer)
{
  int drum_type_to_add;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    drum_type_to_add = selectDrumLoop(string_pointer);

    switch(drum_type_to_add) {
      case DRUM_KICK: audio_addLoop(DRUM_KICK); break;
      case DRUM_CLAP: audio_addLoop(DRUM_CLAP); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void addBass(char *string_pointer)
{
  int bass_type_to_add;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    bass_type_to_add = selectBassLoop(string_pointer);

    switch(bass_type_to_add) {
      case BASS_1: audio_addLoop(BASS_1); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void addKeys(char *string_pointer)
{
  int keys_type_to_add;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    keys_type_to_add = selectKeysLoop(string_pointer);

    switch(keys_type_to_add) {
      case KEYS_1: audio_addLoop(KEYS_1); break;
    }
  }
}

void remove_(char *string_pointer)
{
  int instrument_to_remove;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    printf("TEXT: Remove function: Instrument: %s\n", string_pointer);

    instrument_to_remove = selectInstrument(string_pointer);
    printf("instrument_to_remove: %d\n", instrument_to_remove);

    switch(instrument_to_remove) {
      case DRUM: removeDrum(string_pointer); break;
      case BASS: removeBass(string_pointer); break;
      case KEYS: removeKeys(string_pointer); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void removeDrum(char *string_pointer)
{
  int drum_type_to_remove;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    drum_type_to_remove = selectDrumLoop(string_pointer);

    switch(drum_type_to_remove) {
      case DRUM_KICK: audio_markLoopInactive(DRUM_KICK); break;
      case DRUM_CLAP: audio_markLoopInactive(DRUM_CLAP); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void removeBass(char *string_pointer)
{
  int bass_type_to_remove;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    bass_type_to_remove = selectBassLoop(string_pointer);

    switch(bass_type_to_remove) {
      case BASS_1: audio_markLoopInactive(BASS_1); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void removeKeys(char *string_pointer)
{
  int keys_type_to_remove;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    keys_type_to_remove = selectKeysLoop(string_pointer);

    switch(keys_type_to_remove) {
      case KEYS_1: audio_markLoopInactive(KEYS_1); break;
    }
  }
}

void set_(char *string_pointer)
{
  int set_instrument_volume;
  char *loop_type, *command;
  string_pointer = strtok(NULL, " \n.()");

  if(string_pointer){
    // temp now points to "volume".
    loop_type = strtok(NULL, " \n.()");
    command = strtok(NULL, " \n.()");
    /*
    printf("loop_type:%s\n", loop_type);
    printf("command:%s\n", command);
    */

    if (strcmp(command, "volume") == 0){
      /*
      printf("string b4 instrument switch in set: %s\n", string_pointer);
      */
      set_instrument_volume = selectInstrument(string_pointer);

      //switch only compares earlier pointer - which hopefully
      //points to the instrument
      switch(set_instrument_volume){
        case DRUM: setDrumVolume(loop_type); break;
        case BASS: setBassVolume(loop_type); break;
        case KEYS: setKeysVolume(loop_type); break;
      }
    }
  }
}
// No need for switch - already have data so just call the function!
void setDrumVolume(char *string_pointer)
{
  int drum_type_to_set_volume;
  /*
  printf("receieved strng in setDrumVolume: %s\n", string_pointer);
  Strtok here doesn't get (kick) or any loop types in brackets. Have
  tried changing delimiters as well as strtok'ing again but no luck...
  string_pointer = strtok(NULL, " \n.()");

  printf("strng in setDrumVolume b4 switch: %s\n", string_pointer);
  */
  if(string_pointer){
    drum_type_to_set_volume = selectDrumLoop(string_pointer);

    switch(drum_type_to_set_volume) {
      case DRUM_KICK: changeVolume(string_pointer, DRUM_KICK); break;
      case DRUM_CLAP: changeVolume(string_pointer, DRUM_CLAP); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void setBassVolume(char *string_pointer)
{
  int bass_type_to_set_volume;

  if(string_pointer){
    bass_type_to_set_volume = selectBassLoop(string_pointer);

    switch(bass_type_to_set_volume) {
      case BASS_1: changeVolume(string_pointer, BASS_1); break;
    }
  }
}
// No need for switch - already have data so just call the function!
void setKeysVolume(char *string_pointer)
{
  int keys_type_to_set_volume;

  if(string_pointer){
    keys_type_to_set_volume = selectKeysLoop(string_pointer);

    switch(keys_type_to_set_volume) {
      case KEYS_1: changeVolume(string_pointer, KEYS_1); break;
    }
  }
}

void changeVolume(char *string_pointer, Loop index)
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

void stopAll_(char *string_pointer)
{
  if(string_pointer){
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

int selectInstrument(char *string)
{
  printf("TEXT: Select instrument string: %s\n", string);
  if(strcmp(string, "drum") == 0){
     printf("TEXT: drum\n");
     return DRUM;
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

int selectDrumLoop(char *string)
{
  printf("Select Drum Loop string: %s\n", string);
  if(strcmp(string, "kick") == 0){
     printf("TEXT: drum(kick)\n");
     return DRUM_KICK;
  }
  if(strcmp(string, "clap") == 0){
     printf("TEXT: drum(clap)\n");
     return DRUM_CLAP;
  }
  return -1;
}

int selectBassLoop(char *string)
{
  if(strcmp(string, "1") == 0){
     printf("TEXT: 1\n");
     return BASS_1;
  }
  return -1;
}

int selectKeysLoop(char *string)
{
  if(strcmp(string, "1") == 0){
     printf("TEXT: 1\n");
     return KEYS_1;
  }
  return -1;
}

char *createStringCopy(char *input_string)
{
  char *copy;

  copy = malloc((strlen(input_string) * sizeof(char)) + NULLCHAR);
  if(!copy) {
    fprintf(stderr, "Could not allocate string copy.\n");
    exit(1);
  }
  strcpy(copy, input_string);
  return copy;
}
