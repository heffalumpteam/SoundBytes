#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <string.h>

#include "text.h"
#include "events.h"

void addLoop(char *p);

void text_receiveUpdate(char *s){
  char* p;
  int i = 1;

/*Takes input line from GUI and splits into individual words*/
  p = strtok(s, " \n.()");
  do{
    printf("Instruction %d Received: %s\n", i, p);
    if(strcmp(p, "add") == 0){
      addLoop(p);
    }
    if(strcmp(p, "stop") == 0){
      printf("Stop!\n");
    }
    p = strtok(NULL, " \n.()");
    i++;
  }while(p);
}

void addLoop(char *p){
  char *s;
  p = strtok(NULL, " \n.()");
  if(p){
    printf("Add function: Instrument: %s\n", p);
    if(strcmp(p, "drums") == 0){
      printf("drums\n");
    }
    if(strcmp(p, "clap") == 0){
      printf("clap\n");
    }
/*If there are other intructions (  on other lines) to carry out, send them back to text_recievUpdate
function to sort out*/
    s = strtok(NULL, "");
    if(s){
      printf("Passed back %s\n", s);
      text_receiveUpdate(s);
    }
  }
}
