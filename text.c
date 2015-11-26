#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <string.h>

#include "text.h"
#include "events.h"

void text_recieveUpdate(char *s){
  char* p, *add = "add", *stop = "stop";
  int i = 1;

/*Takes input line from GUI and splits into individual words*/
  p = strtok(s, " \n.()");
  do{
    printf("Instruction %d Received: %s\n", i, p);
    if(strcmp(p, add) == 0){
      text_add(p);
    }
    if(strcmp(p, stop) == 0){
      events_stop();
    }
    p = strtok(NULL, " \n.()");
    i++;
  }while(p);
}

void text_add(char *p){
  char *s, *drums = "drums", *clap = "clap";
  p = strtok(NULL, " \n.()");
  if(p){
    printf("Add function: Instrument: %s\n", p);
    if(strcmp(p, drums) == 0){
/*For now start the loop, but when we have more loops, call each individually
  depending on keyword*/
      events_start(); 
    }
    if(strcmp(p, clap) == 0){
      printf("clap\n");
      events_clap1();
    }
/*If there are other intructions (on other lines) to carry out, send them back to text_recievUpdate
function to sort out*/
    s = strtok(NULL, "");
    if(s){
      printf("Passed back %s\n", s);
      text_recieveUpdate(s);
    }
  }
}