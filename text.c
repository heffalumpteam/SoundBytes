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
void changeVolume(char *string_pointer, char *desiredVolume);
void stopAll_(void);

/* Helpers */
int extractNumberFromString(char *string);
char *findSampleInArray(char *string_pointer);

//-------------------------------------------------------

// Interrupt for audio mainloop. Required to pass through text.c to avoid
// changes to program structure and including audio.h in events.c Keeps
// program linear.
void text_mainLoop(void) {
  audio_mainLoop();
}

//-------------------------------------------------------

//Function for lefthand button press
void text_receiveButtonPress(char *input_string) {
  audio_playSampleOnce(extractNumberFromString(input_string));
}

//-------------------------------------------------------

// Function that is called when the 'RUN' command is activated in the GUI.
// Makes everything lowercase and tokenizes the first instruction, passing
// it to the "instructionControl" function.
void text_receiveUpdate(char *input_string) {
  int j;
  char *string_pointer = NULL;

  for (j= 0; input_string[j] != '\0'; j++) {
     input_string[j] = tolower(input_string[j]);
   }

  string_pointer = strtok(input_string, " \n");

  while(string_pointer) {
    instructionControl(string_pointer);
    string_pointer = strtok(NULL, " \n");
  }
}

// Tokenizes the instruction part of the text command. If it corresponds to one of the supported
// instructions, like "add" or "remove", it searches for the next token (the sample name) in the
// list of samples and sends its buttonID to the relevant instruction function for further processing
void instructionControl(char *string_pointer) {
  char *buttonID = NULL;
  int instruction_to_execute;

  if (!string_pointer) {
    return;
  }

  instruction_to_execute = selectInstruction(string_pointer);

  string_pointer = strtok(NULL, " \n");

  if (string_pointer != NULL) {
    buttonID = findSampleInArray(string_pointer); //
  }

  switch(instruction_to_execute) {
    case ADD: add_(buttonID); break;
    case REMOVE: remove_(buttonID); break;
    case SET: set_(string_pointer); break;
    case STOP: stopAll_(); break;
  }
}

int selectInstruction(char *string) {
  if(strcmp(string, "add") == 0){
    return ADD;
  }
  if(strcmp(string, "remove") == 0) {
    return REMOVE;
  }
  if(strcmp(string, "set") == 0) {
    return SET;
  }
  if(strcmp(string, "stop") == 0) {
    return STOP;
  }
  return -1;
}

// Searches the array of buttons/samples, comparing the sample name given in the instruction
// to the names in the array. If there's a match, the buttonID is returned to later be sent to
// audio.c for processing.
char *findSampleInArray(char *string_pointer) {
  int i;

  for(i=0; i < audio_noOfSamplesLoaded; i++) {
    if (strcmp(string_pointer, buttons[i].sampleName) == 0) {
      return buttons[i].buttonID;
    }
  }

  return NULL;
}

void add_(char *buttonID) {

  if (buttonID) {
    audio_addLoop(extractNumberFromString(buttonID));
  }
}

void remove_(char *buttonID) {

  if (buttonID) {
    audio_markLoopInactive(extractNumberFromString(buttonID));
  }
}

// Decodes the set volume instruction through tokens and sends the buttonID of the sample specified (if it exists)
// through to the changeVolume function
void set_(char *string_pointer) {
  char *volumeCommand, *desiredVolume;
  char *buttonID;

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

void stopAll_(void) {
    audio_stop();
}

//-------------------------------------------------------

void changeVolume(char *string_pointer, char *desiredVolume) {
  int volume;

  // Convert the string to an int
  volume = atoi(desiredVolume);

  // The user entered a valid volume number
  if ((volume > 0) && (volume < 12)) {
    volume = ((128 / 11) * volume); // Scales the volume to the range 0-128
    audio_changeVolume(extractNumberFromString(string_pointer), volume);
  }
  else {
    fprintf(stderr, "Next time please enter a number between 0 and 11!\n");
  }
}

// Mainly used for extracting the number from buttonID strings to send to audio.c
int extractNumberFromString(char *string) {
  int index;

  while(!isdigit(*string)) {
    string++;
  }

  sscanf(string, "%d", &index);

  return index;
}

//-------------------------------------------------------
