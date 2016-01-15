#ifndef EVENTS_H_
#define EVENTS_H_

#define NUM_MS 1

gboolean events_mainLoop(gpointer user_data);
void* events_secondLoop(void*);
void events_drum1(void);
void events_clap1(void);
void events_toggle(void);
void events_start(void);
void events_stop(void);
void events_quitting(void);
void events_launchText(GtkSourceBuffer *sourcebuffer);
void events_openFile(char* filename, GtkSourceBuffer *sourcebuffer);
void events_saveFile(char* filename, GtkSourceBuffer *sourcebuffer);
void events_buttonPress(GtkButton *button, gpointer data);
void events_init(GtkSourceBuffer* sourcebuffer);

#endif
