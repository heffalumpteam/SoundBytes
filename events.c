#include <gtk/gtk.h> //needed for gboolean and gpointer

#include "events.h"
#include "graphics.h"
#include "audio.h"

#define BPM 120
#define MSperBEAT 500
#define BEATS_IN_A_BAR 4

unsigned char beat = 0, bar = 0;

gboolean events_mainLoop(gpointer user_data){
  /*https://developer.gnome.org/gtk-tutorial/stable/c1759.html*/
  /*This loop runs every NUM_MS and takes the place of code that would usually be in main()*/
  if(beat > BEATS_IN_A_BAR-1){
    bar++;
    beat = 0;
  }
  printf("Bar: %d Beat: %d\n", bar, beat);
  //events_drum1();
  beat++;
  return 1; /*Must return 1 if we want the loop to be called again*/
}

void events_drum1(void){
  audio_play1();
}

void events_clap1(void){
  audio_play2();
}

void events_quitting(void){
  audio_close();
  graphics_close();
}
