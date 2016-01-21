#include <gtk/gtk.h>
#include <glib.h>
#include <string.h> //for the CSS loading
//  https://wiki.gnome.org/Projects/GtkSourceView   https://github.com/GNOME/gtksourceview
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "graphics.h"
#include "events.h"
#include "samples.h"

#define UNUSED(x) (void)(x)
#define MAX_BUTTON_ID_LENGTH 10
#define VERY_HIGH_PRIORITY -200

extern unsigned char running;

GtkSourceBuffer *sourcebuffer;
gchar languagesPath[] = "lang/language-specs/"; /* Custom language file for syntax highlighting */
gchar* languagesDirs[] = {languagesPath, NULL};

/* Setup */
void initSourceView(GtkBuilder *builder);
void attachFunctions(GtkBuilder *builder);
GtkButton* setUpGtkButton(GtkBuilder *builder, char* buttonID, void (*function)(GtkButton*, gpointer));
void setUpPreviewButtons(GtkBuilder *builder);
/* Saving and loading .lump files */
void openFileDialog(GtkButton *button, GtkBuilder *builder);
void saveFileDialog(GtkButton *button, GtkBuilder *builder);
void loadHelpFile(void);
/* Helper functions */
char* extractFilenameFromPath(char* path);
gchar* createButtonIDForIndex(int i);
void makeLowercase(char* filename);
/* Misc */
void launchTextEvent(void);
void style(void);


void graphics_init(void) {
  GtkBuilder *builder;

  gtk_init (NULL, NULL);

  builder = gtk_builder_new_from_file ("graphicsFiles/ui.ui");

  attachFunctions(builder);
  initSourceView(builder);

  events_init(sourcebuffer);

  style();
  gtk_main ();
}

gchar* createButtonIDForIndex(int i) {
  char* buffer;

  buffer = malloc(MAX_BUTTON_ID_LENGTH * sizeof *buffer);
  sprintf(buffer, "button%d", i);
  return (gchar*)buffer;
}

void initSourceView(GtkBuilder *builder) {
  GtkSourceView *sourceview = GTK_SOURCE_VIEW(gtk_builder_get_object (builder, "gtksourceview1"));
  sourcebuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview)));
  GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
  gtk_source_language_manager_set_search_path(manager, languagesDirs);
  GtkSourceLanguage *language = gtk_source_language_manager_get_language(manager, "heffalump");
  gtk_source_buffer_set_language(sourcebuffer, language);
}

void attachFunctions(GtkBuilder *builder) {
  GObject *window;
  GtkButton *runButton;
  GtkButton *openButton;
  GtkButton *saveButton;
  GtkButton *helpButton;
  guint timeoutID;
  GtkWidget *icon = gtk_image_new_from_file ("graphicsFiles/icons/start.png");
  assert(icon != NULL);

  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (events_quitting), NULL);

  /*timeoutID = g_timeout_add(NUM_MS, events_mainLoop, NULL);*/
  timeoutID = g_timeout_add_full(VERY_HIGH_PRIORITY, NUM_MS, events_mainLoop, NULL, NULL);
  assert(timeoutID > 0);

  setUpPreviewButtons(builder);

  runButton = (GtkButton *)gtk_builder_get_object (builder, "runButton");
  g_signal_connect (runButton, "clicked", G_CALLBACK (launchTextEvent), NULL);

  openButton = (GtkButton *)gtk_builder_get_object (builder, "openButton");
  g_signal_connect (openButton, "clicked", G_CALLBACK (openFileDialog), builder);

  saveButton = (GtkButton *)gtk_builder_get_object (builder, "saveButton");
  g_signal_connect (saveButton, "clicked", G_CALLBACK (saveFileDialog), builder);

  helpButton = (GtkButton *)gtk_builder_get_object (builder, "helpButton");
  g_signal_connect (helpButton, "clicked", G_CALLBACK (loadHelpFile), NULL);
}

GtkButton* setUpGtkButton(GtkBuilder *builder, char* buttonID, void (*function)(GtkButton*, gpointer)) {

  GtkButton* button;

  button = GTK_BUTTON(gtk_builder_get_object(builder, buttonID));
  g_signal_connect(button, "clicked", G_CALLBACK(function), buttonID);

  return button;
}

void setUpPreviewButtons(GtkBuilder *builder) {
  int i = 0, filenameLength;
  GtkButton *button;
  char* filename;

  while(sampleFilePaths[i]) {
    buttons[i].buttonID = createButtonIDForIndex(i);
    button = setUpGtkButton(builder, buttons[i].buttonID, events_buttonPress);
    assert(button != NULL);

    filename = extractFilenameFromPath(sampleFilePaths[i]);
    filenameLength = strlen(filename);

    if (filenameLength > MAXFILENAMELENGTH) {
      filename[MAXFILENAMELENGTH - 4] = '.';
      filename[MAXFILENAMELENGTH - 3] = '.';
      filename[MAXFILENAMELENGTH - 2] = '.';
      filename[MAXFILENAMELENGTH - 1] = '\0';
    }

    makeLowercase(filename);

    strcpy(buttons[i].sampleName, filename);
    gtk_button_set_label (button, filename);

    i++;
  }
}

void makeLowercase(char* filename) {

  int j = 0;

  while(filename[j] != '\0') {
      filename[j] = tolower(filename[j]);
      j++;
    }
}

char* extractFilenameFromPath(char* path) {
  char* token;
  char* temp = malloc(strlen(path) + 1);

  strcpy(temp, path);

  token = strchr(temp, '/');
  token++;
  token = strtok(token, ".");

  free(temp);

  return token;
}

void launchTextEvent(void) {
  /*Seems a necceserry hack. It doesn't like passing sourcebuffer as arg to callback*/
  events_launchText(sourcebuffer);
}

void openFileDialog(GtkButton *button, GtkBuilder *builder) {
  GObject *window;
  GtkWidget *open_dialog;
  GtkFileFilter *filter;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint result;

  UNUSED(button);
  filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.lump");
  gtk_file_filter_set_name(filter, "*.lump");
  window = gtk_builder_get_object(builder, "window");
  open_dialog = gtk_file_chooser_dialog_new("Open File", (GtkWindow*) window, action, ("Cancel"), GTK_RESPONSE_CANCEL,("Open"),
    GTK_RESPONSE_ACCEPT, NULL);
  GtkFileChooser *chooser = GTK_FILE_CHOOSER (open_dialog);
  gtk_file_chooser_add_filter(chooser, filter);
  gtk_file_chooser_set_filter(chooser, filter);
  result = gtk_dialog_run(GTK_DIALOG (open_dialog));

  if (result == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = (char*)gtk_file_chooser_get_filename(chooser);
    events_openFile(filename, sourcebuffer);
    free(filename);
  }
  gtk_widget_destroy(open_dialog);
}

void saveFileDialog(GtkButton *button, GtkBuilder *builder) {
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

  if (result == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = (char*)gtk_file_chooser_get_filename(chooser);
    events_saveFile(filename, sourcebuffer);
    free(filename);
  }
  gtk_widget_destroy(save_dialog);
}

void loadHelpFile(void) {
  char* filename = "documentation/helpFile.txt";
  events_openFile(filename, sourcebuffer);
}

void style(void) {
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

void graphics_close(void) {
  gtk_main_quit();
}
