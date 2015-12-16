#ifndef _events
#define _events

#define NUM_MS 500

gboolean events_mainLoop(gpointer user_data);
void events_drum1(void);
void events_clap1(void);
void events_toggle(void);
void events_start(void);
void events_stop(void);
void events_quitting(void);
void events_launchText(GtkSourceBuffer *sourcebuffer);
void events_buttonPress(GtkButton *);
void events_init(GtkSourceBuffer* sourcebuffer);

#endif
