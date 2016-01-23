#include <stdio.h>
#include "graphics.h"
#include "audio.h"

int main () {

  audio_init();
  graphics_init();

  audio_close();
  
  return 0;
}
