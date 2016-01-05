#include <gtk/gtk.h>
#include <glib.h>
#include <string.h> //for the CSS loading
//  https://wiki.gnome.org/Projects/GtkSourceView   https://github.com/GNOME/gtksourceview
#include <assert.h>
#include <stdlib.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "graphics.h"
#include "events.h"

#define UNUSED(x) (void)(x)

extern unsigned char running;
GtkSourceBuffer *sourcebuffer;
gchar languagesPath[] = "lang/language-specs/";
gchar* languagesDirs[] = {languagesPath, NULL};

void initSourceView(GtkBuilder *builder);
void attachFunctions(GtkBuilder *builder);
void launchTextEvent(void);
void openFileDialog(GtkButton *button, GtkBuilder *builder);
void saveFileDialog(GtkButton *button, GtkBuilder *builder);
void openFile(char* filename);
int fileLength(FILE* f_input);
void style(void);
GtkButton* setUpGtkButton(GtkBuilder *builder, char* buttonID, void (*function)(GtkButton*));

void graphics_init(void){
  GtkBuilder *builder;

  gtk_init (NULL, NULL);

  builder = gtk_builder_new_from_file ("graphicsFiles/ui.ui");

  attachFunctions(builder);
  initSourceView(builder);

  events_init(sourcebuffer);

  style();

  // g_object_unref( G_OBJECT( builder ) );
  gtk_main ();
}

void initSourceView(GtkBuilder *builder){
  GtkSourceView *sourceview = GTK_SOURCE_VIEW(gtk_builder_get_object (builder, "gtksourceview1"));
  sourcebuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview)));
  GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
  gtk_source_language_manager_set_search_path(manager, languagesDirs);
  GtkSourceLanguage *language = gtk_source_language_manager_get_language(manager, "heffalump");
  gtk_source_buffer_set_language(sourcebuffer, language);
}

void attachFunctions(GtkBuilder *builder){
  GObject *window;
  GtkButton *button1;
  GtkButton *button2;
  GtkButton *button3;
  GtkButton *button4;
  GtkButton *runButton;
  GtkButton *openButton;
  GtkButton *saveButton;
  guint timeoutID;
  GtkWidget *icon = gtk_image_new_from_file ("graphicsFiles/icons/start.png");
  assert(icon != NULL);

  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (events_quitting), NULL);

  timeoutID = g_timeout_add(NUM_MS, events_mainLoop, NULL);
  assert(timeoutID > 0);

  button1 = setUpGtkButton(builder, "button1", events_buttonPress); /* Generic function, see below */
  assert(button1 != NULL);
  button2 = setUpGtkButton(builder, "button2", events_buttonPress);
  assert(button2 != NULL);
  button3 = setUpGtkButton(builder, "button3", events_buttonPress); /* Generic function, see below */
  assert(button3 != NULL);
  button4 = setUpGtkButton(builder, "button4", events_buttonPress);
  assert(button4 != NULL);

  runButton = (GtkButton *)gtk_builder_get_object (builder, "runButton");
  g_signal_connect (runButton, "clicked", G_CALLBACK (launchTextEvent), NULL);

  openButton = (GtkButton *)gtk_builder_get_object (builder, "openButton");
  g_signal_connect (openButton, "clicked", G_CALLBACK (openFileDialog), builder);

  saveButton = (GtkButton *)gtk_builder_get_object (builder, "saveButton");
  g_signal_connect (saveButton, "clicked", G_CALLBACK (saveFileDialog), builder);
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

void openFileDialog(GtkButton *button, GtkBuilder *builder)
{
  GObject *window;
  GtkWidget *open_dialog;
  GtkFileFilter *filter, *filterAll;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint result;

  UNUSED(button);
  filter = gtk_file_filter_new();
  filterAll = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.lump");
  gtk_file_filter_set_name(filter, "*.lump");
  gtk_file_filter_add_pattern(filterAll, "*");
  gtk_file_filter_set_name(filterAll, "All files");
  window = gtk_builder_get_object(builder, "window");
  open_dialog = gtk_file_chooser_dialog_new("Open File", (GtkWindow*) window, action, ("Cancel"), GTK_RESPONSE_CANCEL,("Open"),
    GTK_RESPONSE_ACCEPT, NULL);
  GtkFileChooser *chooser = GTK_FILE_CHOOSER (open_dialog);
  gtk_file_chooser_add_filter(chooser, filter);
  gtk_file_chooser_add_filter(chooser, filterAll);
  gtk_file_chooser_set_filter(chooser, filter);
  result = gtk_dialog_run(GTK_DIALOG (open_dialog));

  if (result == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    filename = (char*)gtk_file_chooser_get_filename(chooser);
    events_openFile(filename, sourcebuffer);
    free(filename);
  }
  gtk_widget_destroy(open_dialog);
}

void saveFileDialog(GtkButton *button, GtkBuilder *builder)
{
  GObject *window;
  GtkWidget *save_dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint result;

  UNUSED(button);
  window = gtk_builder_get_object(builder, "window");
  save_dialog = gtk_file_chooser_dialog_new("Save File", (GtkWindow*) window, action, ("Cancel"), GTK_RESPONSE_CANCEL, ("_Save"),
    GTK_RESPONSE_ACCEPT, NULL);
  
  chooser = GTK_FILE_CHOOSER(save_dialog);
  gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

  gtk_file_chooser_set_current_name(chooser,("mytune.lump"));

  result = gtk_dialog_run(GTK_DIALOG(save_dialog));

  if (result == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    filename = (char*)gtk_file_chooser_get_filename(chooser);
    events_saveFile(filename, sourcebuffer);
    free(filename);
  }
  gtk_widget_destroy(save_dialog);
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
