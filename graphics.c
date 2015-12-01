#include <gtk/gtk.h>
#include <glib.h>
#include <string.h> //for the CSS loading
//  https://wiki.gnome.org/Projects/GtkSourceView   https://github.com/GNOME/gtksourceview

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "graphics.h"
#include "events.h"

extern unsigned char running;
GtkSourceBuffer *sourcebuffer;
gchar languagesPath[] = "lang/language-specs/";
gchar* languagesDirs[] = {languagesPath, NULL};

void initSourceView(GtkBuilder *builder);
void attachFunctions(GtkBuilder *builder);
void launchTextEvent(void);
void style(void);
GtkButton* setUpGtkButton(GtkBuilder *builder, char* buttonID, void (*function)(GtkButton*));

void graphics_init(void){
  GtkBuilder *builder;

  gtk_init (NULL, NULL);

  builder = gtk_builder_new_from_file ("graphicsFiles/ui.ui");

  attachFunctions(builder);
  initSourceView(builder);

  style();

  g_object_unref( G_OBJECT( builder ) );
  gtk_main ();
}

void initSourceView(GtkBuilder *builder){
  GtkSourceView *sourceview = GTK_SOURCE_VIEW(gtk_builder_get_object (builder, "gtksourceview1"));
  sourcebuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview)));
  GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
  gtk_source_language_manager_set_search_path(manager, languagesDirs);
  GtkSourceLanguage *language = gtk_source_language_manager_get_language(manager, "heffalump");
  gtk_source_buffer_set_language(sourcebuffer, language);

  /*gtk_text_buffer_set_text(GTK_TEXT_BUFFER(sourcebuffer),
      "Type stop and press run to stop the beat\n", -1);*/

  /*g_signal_connect(G_OBJECT(sourcebuffer), "changed", G_CALLBACK(events_textChanged), sourcebuffer);*/
}

void attachFunctions(GtkBuilder *builder){
  GObject *window;
  GtkButton *button1;
  GtkButton *button2;
  GtkButton *playtoggle;
  GtkButton *runButton;
  guint timeoutID;
  GtkWidget *icon = gtk_image_new_from_file ("graphicsFiles/icons/start.png");

  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (events_quitting), NULL);

  timeoutID = g_timeout_add(NUM_MS, events_mainLoop, NULL);

  button1 = setUpGtkButton(builder, "button1", events_buttonPress); /* Generic function, see below */
/*  button2 = setUpGtkButton(builder, "button2", events_buttonPress);*/

  playtoggle = (GtkButton *)gtk_builder_get_object (builder, "playToggle");
  g_signal_connect (playtoggle, "clicked", G_CALLBACK (events_toggle), NULL);

  runButton = (GtkButton *)gtk_builder_get_object (builder, "runButton");
  g_signal_connect (runButton, "clicked", G_CALLBACK (launchTextEvent), NULL);

}

GtkButton* setUpGtkButton(GtkBuilder *builder, char* buttonID, void (*function)(GtkButton*)) {

  GtkButton* button;

  button = GTK_BUTTON(gtk_builder_get_object(builder, buttonID));
  g_signal_connect(button, "clicked", G_CALLBACK(function), buttonID);

  return button;
}

void launchTextEvent(void){
  /*Seems a necceserry hack. It doesn't like passing sourcebuffer as arg to callback*/
  events_launchText(sourcebuffer);
}

void style(void){
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
  GError *error = 0;
  gsize bytes_written, bytes_read;
  const gchar* style = "graphicsFiles/style.css";

  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gtk_css_provider_load_from_path (provider, g_filename_to_utf8(style, strlen(style), &bytes_read, &bytes_written, &error), NULL);
}

void graphics_close(void){
  gtk_main_quit();
}
