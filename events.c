#include <gtk/gtk.h>
#include <glib.h>

#include "events.h"
#include "graphics.h"
#include "audio.h"

#define BPM 120
#define MSperBEAT 500

unsigned char beat = 0, bar = 0;

gboolean events_mainLoop(gpointer user_data){
  /*https://developer.gnome.org/gtk-tutorial/stable/c1759.html*/
  /*This loop runs every NUM_MS and takes the place of code that would usually be in main()*/
  printf("Bar: %d Beat: %d\n", bar, beat);
  beat++;
  if(beat > 4){
    bar++;
    beat = 0;
  }
  drum1();
  return 1; /*Must return 1 if we want the loop to be called again*/
}

void drum1(void){
  audio_play1();
}

void clap1(void){
  audio_play2();
}

void quitting(void){
  audio_close();
  graphics_close();
}
