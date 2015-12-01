#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "text.h"
#include "audio.h"

void addRemoveStopLoop(char *str_ptr);

void text_mainLoop(void)
{
  audio_mainLoop();
}

void text_receiveButtonPress(char *input_str)
{
  if((strcmp(input_str, "drum(shuffle)") == 0)){
    audio_playSampleOnce(DRUMS_SHUFFLE);
  }
}

void text_receiveUpdate(char *input_str){
  char* str_ptr;
  int j;
  int i = 1;

  for (j= 0; input_str[j] != '\0'; j++) {
     input_str[j] = tolower(input_str[j]);
  }

/*Takes input line from GUI and splits into individual words*/
  str_ptr = strtok(input_str, " \n.()");
  while(str_ptr){
    printf("Instruction %d Received: %s\n", i, str_ptr);
    if((strcmp(str_ptr, "add") == 0) || (strcmp(str_ptr, "remove") == 0) || \
       (strcmp(str_ptr, "stop") == 0)){
      addRemoveStopLoop(str_ptr);
    }
    str_ptr = strtok(NULL, " \n.()");
    i++;
  }
}

void addRemoveStopLoop(char *str_ptr){
  char *input_str;

  if(strcmp(str_ptr, "add") == 0){
    str_ptr = strtok(NULL, " \n.()");
    if(str_ptr){
      printf("Add function: Instrument: %s\n", str_ptr);

      if(strcmp(str_ptr, "drums") == 0){
         printf("drums\n");
         audio_addLoop(DRUMS_SHUFFLE);
      }
      if(strcmp(str_ptr, "clap") == 0){
         printf("clap\n");
         audio_addLoop(DRUMS_CLAP);
      }
    }
  }
  if (strcmp(str_ptr, "remove") == 0){
    str_ptr = strtok(NULL, " \n.()");
    if(str_ptr){
      printf("Remove function: Instrument: %s\n", str_ptr);

      if(strcmp(str_ptr, "drums") == 0){
        printf("drums\n");
        audio_markLoopInactive(DRUMS_SHUFFLE);
      }
      if(strcmp(str_ptr, "clap") == 0){
        printf("clap\n");
        audio_markLoopInactive(DRUMS_CLAP);
      }
    }
  }
  if (strcmp(str_ptr, "stop") == 0){
    if(str_ptr){
      audio_stop();
      printf("All loops stopped.\n");
    }
  }

//If there are other intructions (  on other lines) to carry out, send
//them back to text_recievUpdate function to sort out
    input_str = strtok(NULL, "");
    //s = strtok(NULL, " ");
    if(input_str){
      printf("Passed back %s\n", input_str);
      text_receiveUpdate(input_str);
    }
}
