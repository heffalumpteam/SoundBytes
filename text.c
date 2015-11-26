#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "text.h"
#include "audio.h"

void addRemoveLoop(char *p);

void text_receiveUpdate(char *s){
  char* p;
  int j;
  int i = 1;

  for (j= 0; s[j] != '\0'; j++) {
     s[j] = tolower(s[j]);
 }

/*Takes input line from GUI and splits into individual words*/
  p = strtok(s, " \n.()");
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
  char *s;

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
        audio_removeLoop(DRUMS_SHUFFLE);
      }
      if(strcmp(p, "clap") == 0){
        printf("clap\n");
        audio_removeLoop(DRUMS_CLAP);
      }
    }
  }
/*If there are other intructions (  on other lines) to carry out, send them back to text_recievUpdate
function to sort out*/

    //Need to fix whitespace bug!! getchar? then return
    //pointer to first char

    s = strtok(NULL, "");
    //s = strtok(NULL, " ");
    if(s){
      printf("Passed back %s\n", s);
      text_receiveUpdate(s);
    }
}
