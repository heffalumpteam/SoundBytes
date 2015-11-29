#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "text.h"
#include "audio.h"

void addRemoveLoop(char *p);

void text_mainLoop(void)
{
  audio_mainLoop();
}

void text_receiveUpdate(char *sample){
  char* p;
  int j;
  int i = 1;

  for (j= 0; sample[j] != '\0'; j++) {
     sample[j] = tolower(sample[j]);
 }

/*Takes input line from GUI and splits into individual words*/
  p = strtok(sample, " \n.()");
  do{
    printf("Instruction %d Received: %s\n", i, p);
    if((strcmp(p, "add") == 0) || (strcmp(p, "remove") == 0)){
      addRemoveLoop(p);
    }
    p = strtok(NULL, " \n.()");
    i++;
  }while(p);
}

void addRemoveLoop(char *p){
  char *sample;

  if(strcmp(p, "add") == 0){
    p = strtok(NULL, " \n.()");
    if(p){
      printf("Add function: Instrument: %s\n", p);

      if(strcmp(p, "drums") == 0){
         printf("drums\n");
         audio_addLoop(DRUMS_SHUFFLE);
      }
      if(strcmp(p, "clap") == 0){
         printf("clap\n");
         audio_addLoop(DRUMS_CLAP);
      }
    }
  }
  if (strcmp(p, "remove") == 0){
    p = strtok(NULL, " \n.()");
    if(p){
      printf("Remove function: Instrument: %s\n", p);

      if(strcmp(p, "drums") == 0){
        printf("drums\n");
        audio_markLoopInactive(DRUMS_SHUFFLE);
      }
      if(strcmp(p, "clap") == 0){
        printf("clap\n");
        audio_markLoopInactive(DRUMS_CLAP);
      }
    }
  }
/*If there are other intructions (  on other lines) to carry out, send them back to text_recievUpdate
function to sort out*/

    //Need to fix whitespace bug!! getchar? then return
    //pointer to first char

    sample = strtok(NULL, "");
    //s = strtok(NULL, " ");
    if(sample){
      printf("Passed back %s\n", sample);
      text_receiveUpdate(sample);
    }
}
