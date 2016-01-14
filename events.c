#include <gtk/gtk.h> //needed for gboolean and gpointer

#include <gtksourceview/gtksourcebuffer.h>

#include <stdlib.h>
#include <string.h>

#include "events.h"
#include "graphics.h"
#include "audio.h"
#include "text.h"

#define BPM 125
#define MSperBEAT 480
#define BEATS_IN_A_BAR 4

unsigned char beat = 0, bar = 0;
unsigned int play = 0;

GtkTextMark* textMarker;

int fileLength(FILE* f_input);

gboolean events_mainLoop(gpointer user_data) {
  /*https://developer.gnome.org/gtk-tutorial/stable/c1759.html*/
  /*This loop runs every NUM_MS and takes the place of code that would usually be in main()*/
  if(user_data != NULL){
    fprintf(stderr, "\n\tError: UserData in events_mainLoop was not NULL\n");
  }
  if(play){
    if(beat > BEATS_IN_A_BAR-1){
      bar++;
      beat = 0;
      text_mainLoop();
    }
    /*printf("Bar: %d Beat: %d\n", bar, beat);*/
    beat++;
  }
  return 1; /*Must return 1 if we want the loop to be called again*/
}

void events_toggle(void) {
  if(!play) {
    play = 1;
  } 
  else {
    play = 0;
  }
}

void events_buttonPress(GtkButton *button, gpointer buttonID) {
  (void) button;
  text_receiveButtonPress(buttonID);
}

void events_start(void) {
  play = 1;
}

void events_stop(void) {
  play = 0;
}

void events_init(GtkSourceBuffer* sourcebuffer) {
  GtkTextIter start;
  gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(sourcebuffer), &start, 0);
  textMarker = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(sourcebuffer),
                            NULL,
                            &start,
                           TRUE);

  events_toggle();
}

void events_openFile(char* filename, GtkSourceBuffer *sourcebuffer) {
  FILE* f_input = NULL;
  char *contents;
  int length = 0;

  if((f_input = fopen(filename, "r"))){
    length = fileLength(f_input);
    contents = (char*)calloc(length, sizeof(char));
    rewind(f_input);
    fread(contents, sizeof(char), length, f_input);
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(sourcebuffer), contents, -1);
    fclose(f_input);
    free(contents);  
  }
  else{
    printf("Couldn't open file\n");
  }
}

void events_saveFile(char* filename, GtkSourceBuffer *sourcebuffer) {
  FILE* f_output = NULL;
  GtkTextIter start, end;
  char* buffer;

  gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(sourcebuffer), &start, &end);
  buffer = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(sourcebuffer), &start, &end, FALSE);

  if((f_output = fopen(filename, "w"))) {
    fputs(buffer, f_output);
    fclose(f_output);
  }
  else {
    printf("Couldn't write file\n");
  }
}

int fileLength(FILE* f_input) {
  int count = 0;

  while(!feof(f_input)) {
    getc(f_input);
    count++;
  }
  return count;
}

void events_launchText(GtkSourceBuffer *sourcebuffer) {
  /*
https://git.gnome.org/browse/gtk+/tree/demos/gtk-demo/textview.c */

  GtkTextIter start, end;
  gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(sourcebuffer), &start, textMarker);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(sourcebuffer), &end, 3);
  gtk_text_iter_forward_to_end (&end);
  text_receiveUpdate((char *)gtk_text_iter_get_text(&start, &end));
  gtk_text_buffer_move_mark(GTK_TEXT_BUFFER(sourcebuffer), textMarker, &end);
}

void events_quitting(void) {
  audio_close();
  graphics_close();
}
