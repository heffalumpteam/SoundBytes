#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "text.h"
#include "audio.h"
#include "samples.h"

enum instruction {
  ADD,
  REMOVE,
  SET,
  STOP
};
typedef enum instruction Instruction;

/* Instructions */
int selectInstruction(char *string);
void instructionControl(char *string_pointer);
void add_(char *string_pointer);
void remove_(char *string_pointer);
void set_(char *string_pointer);
void changeVolume(char *string_pointer, char* desiredVolume);
void stopAll_(void);
/* Helpers */
int extractNumberFromString(char* string);
char* findSampleInArray(char* string_pointer);

void text_mainLoop(void) {
  audio_mainLoop();
}

void text_receiveButtonPress(char *input_string) {
  audio_playSampleOnce(extractNumberFromString(input_string));
}

void text_receiveUpdate(char *input_string) {
  int j;
  char* string_pointer = NULL;

  for (j= 0; input_string[j] != '\0'; j++) {
     input_string[j] = tolower(input_string[j]);
   }

  string_pointer = strtok(input_string, " \n");

  while(string_pointer) {
    instructionControl(string_pointer);
    string_pointer = strtok(NULL, " \n");
  }
}

void instructionControl(char *string_pointer) {
  char* buttonID = NULL;
  int instruction_to_execute;

  if (!string_pointer) {
    return;
  }
  
  instruction_to_execute = selectInstruction(string_pointer);

  string_pointer = strtok(NULL, " \n");
  
  if (string_pointer != NULL) {
    buttonID = findSampleInArray(string_pointer);
  }

  switch(instruction_to_execute) {
    case ADD: add_(buttonID); break;
    case REMOVE: remove_(buttonID); break;
    case SET: set_(string_pointer); break;
    case STOP: stopAll_(); break;
  }
}

void add_(char *buttonID) {

  if (buttonID) {
    audio_addLoop(extractNumberFromString(buttonID));
  }
}

char* findSampleInArray(char* string_pointer) {
  int i;

  for(i=0; i < audio_noOfSamplesLoaded; i++) {
    if (strcmp(string_pointer, buttons[i].sampleName) == 0) {
      return buttons[i].buttonID;
    }
  }

  return NULL;
}

void remove_(char *buttonID) {
  
  if (buttonID) {
    audio_markLoopInactive(extractNumberFromString(buttonID));
  }
}

void set_(char *string_pointer) {
  char *volumeCommand, *desiredVolume;
  char* buttonID;

  if(string_pointer) {
    // now points to "volume" and the digit after it.
    volumeCommand = strtok(NULL, " \n.()");
    desiredVolume = strtok(NULL, " \n.()");

    if (strcmp(volumeCommand, "volume") == 0) {

      buttonID = findSampleInArray(string_pointer);
      changeVolume(buttonID, desiredVolume);
    }
  }
}

void changeVolume(char *string_pointer, char* desiredVolume) {
  int volume;

  // Convert the string to an int
  volume = atoi(desiredVolume);

  // The user entered a valid volume number
  if ((volume > 0) && (volume < 12)) {
    printf("atoi check: %d\n", volume);
    volume = ((128 / 11) * volume); // Scales the volume to the range 0-128
    audio_changeVolume(extractNumberFromString(string_pointer), volume);
  }
  else {
    fprintf(stderr, "Next time please enter a number between 0 and 11!\n");
  }
}

void stopAll_(void) {
    audio_stop();
    printf("TEXT: All loops stopped.\n");
}

int selectInstruction(char *string) {
  if(strcmp(string, "add") == 0){
    printf("TEXT: add\n");
    return ADD;
  }
  if(strcmp(string, "remove") == 0) {
    printf("TEXT: remove\n");
    return REMOVE;
  }
  if(strcmp(string, "set") == 0) {
    printf("TEXT: set\n");
    return SET;
  }
  if(strcmp(string, "stop") == 0) {
    printf("TEXT: stop\n");
    return STOP;
  }
  return -1;
}

int extractNumberFromString(char* string) {
  int index;

  while(!isdigit(*string)) {
    string++;
  }

  sscanf(string, "%d", &index);

  return index;
}
