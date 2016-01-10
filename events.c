#include <gtk/gtk.h> //needed for gboolean and gpointer

#include <gtksourceview/gtksourcebuffer.h>


#include "events.h"
#include "graphics.h"
#include "audio.h"
#include "text.h"

#define BPM 120
#define MSperBEAT 500
#define BEATS_IN_A_BAR 4

unsigned char beat = 0, bar = 0;
unsigned int play = 0;

GtkTextMark* textMarker;



gboolean events_mainLoop(gpointer user_data){
  /*https://developer.gnome.org/gtk-tutorial/stable/c1759.html*/
  /*This loop runs every NUM_MS and takes the place of code that would usually be in main()*/
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

void events_toggle(void){
  if(!play){
    play = 1;
  }
  else{
    play = 0;
  }
}

void events_buttonPress(GtkButton *button) {
  char *label;
  label = (char *)gtk_button_get_label(button);
  text_receiveButtonPress(label);
}

void events_start(void){
  play = 1;
}

void events_stop(void){
  play = 0;
}

void events_init(GtkSourceBuffer* sourcebuffer)
{
  GtkTextIter start;
  gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(sourcebuffer), &start, 0);
  textMarker = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(sourcebuffer),
                            NULL,
                            &start,
                           TRUE);

  events_toggle();
}

void events_launchText(GtkSourceBuffer *sourcebuffer){
  /*
https://git.gnome.org/browse/gtk+/tree/demos/gtk-demo/textview.c

  */
  GtkTextIter start, end;
  // gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(sourcebuffer), &start, 0);
  gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(sourcebuffer), &start, textMarker);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(sourcebuffer), &end, 3);
  gtk_text_iter_forward_to_end (&end);
  text_receiveUpdate((char *)gtk_text_iter_get_text(&start, &end));
  gtk_text_buffer_move_mark(GTK_TEXT_BUFFER(sourcebuffer), textMarker, &end);
}

void events_quitting(void){
  audio_close();
  graphics_close();
}
