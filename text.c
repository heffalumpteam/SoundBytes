#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>

#include <stdio.h>
#include <string.h>

#include "text.h"
#include "events.h"

void text_recieveUpdate(char *s){
  char* p;
  int i = 1;
/*Takes input line from GUI and splits into individual words*/
  p = strtok(s, " .()");
   do{
      printf("Instruction %d: %s\n", i, p);
      text_instruction(p);
      p = strtok(NULL, "  .()");
      i++;
   }while(p);
}

void text_instruction(char *p){
	char *stop = "stop";
	if(strcmp(p, stop) == 0){
		printf("stop the beat\n");
		events_start();
	}
}