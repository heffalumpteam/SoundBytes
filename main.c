#include <stdio.h>
#include <glib.h>
#include "graphics.h"
#include "audio.h"

int main () {
  GError *error = NULL;
  //Create new thread
  GThread *thread = g_thread_new("sdl_thread", sdl_thread, NULL);
  if(!thread){
    g_print( "Error: %s\n", error->message );
    return( -1 );
  }

  //audio_init();
  graphics_init();

  //audio_close();

  return 0;
}
