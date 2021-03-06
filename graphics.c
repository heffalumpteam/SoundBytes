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

extern unsigned char running; //Used to quit program

GtkSourceBuffer *sourcebuffer;
gchar languagesPath[] = "lang/language-specs/"; /* Custom language file for syntax highlighting */
gchar *languagesDirs[] = {languagesPath, NULL};


/* Setup */
void initSourceView(GtkBuilder *builder);
void attachFunctions(GtkBuilder *builder);
GtkButton *setUpGtkButton(GtkBuilder *builder, char *buttonID, void (*function)(GtkButton*, gpointer));
void setUpPreviewButtons(GtkBuilder *builder);

/* Saving and loading .lump files */
void openFileDialog(GtkButton *button, GtkBuilder *builder);
void saveFileDialog(GtkButton *button, GtkBuilder *builder);
void loadHelpFile(void);

/* Helper functions */
char *extractFilenameFromPath(char* path);
gchar *createButtonIDForIndex(int i);
void makeLowercase(char *filename);

/* Misc */
void launchTextEvent(void);
void style(void);
void closeGraphics(void);

//-------------------------------------------------------

void graphics_init(void) {
  GtkBuilder *builder;

  //Initialise GTK
  gtk_init (NULL, NULL);

  //Load the UI XML file
  builder = gtk_builder_new_from_file ("graphicsFiles/ui.ui");

  //Attach Functions to Buttons
  attachFunctions(builder);

  //Initialise SourceView - Do the bits that the XML file can't
  initSourceView(builder);

  //Initialise the event handlers
  events_init(sourcebuffer);

  //Load CSS stying on to the GUI
  style();

  //Enter gtk_main loop. Will sit hear until exit.
  gtk_main ();
}

//-------------------------------------------------------

void initSourceView(GtkBuilder *builder) {
  //New Source View Object
  GtkSourceView *sourceview = GTK_SOURCE_VIEW(gtk_builder_get_object (builder, "gtksourceview1"));

  //Assign sourceview object a buffer to write to
  sourcebuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview)));

  //New Language manager for syntax highlighting
  GtkSourceLanguageManager *manager = gtk_source_language_manager_get_default();
  gtk_source_language_manager_set_search_path(manager, languagesDirs);

  //Load language
  GtkSourceLanguage *language = gtk_source_language_manager_get_language(manager, "heffalump");

  //Assign language highlighting to sourcebuffer
  gtk_source_buffer_set_language(sourcebuffer, language);
}

//-------------------------------------------------------

void attachFunctions(GtkBuilder *builder) {
  GObject *window;

  //Buttons on GUI
  GtkButton *runButton;
  GtkButton *openButton;
  GtkButton *saveButton;
  GtkButton *helpButton;

  //Timeout for audio_mainloop
  guint timeoutID;

  //Icon for taskbar
  GtkWidget *icon = gtk_image_new_from_file ("graphicsFiles/icons/start.png");
  assert(icon != NULL);

  //Get window from XML file
  window = gtk_builder_get_object (builder, "window");

  //Assign x button to close window and call closeGraphics function.
  g_signal_connect (window, "destroy", G_CALLBACK (closeGraphics), NULL);

  //Set a timeout for audio_mainloop interupt. Make Very high priority
  timeoutID = g_timeout_add_full(VERY_HIGH_PRIORITY, NUM_MS, events_mainLoop, NULL, NULL);
  assert(timeoutID > 0);

  //Setup Lefthand buttons.
  setUpPreviewButtons(builder);

  //Assign UI buttons to call functions
  runButton = (GtkButton *)gtk_builder_get_object (builder, "runButton");
  g_signal_connect (runButton, "clicked", G_CALLBACK (launchTextEvent), NULL);

  openButton = (GtkButton *)gtk_builder_get_object (builder, "openButton");
  g_signal_connect (openButton, "clicked", G_CALLBACK (openFileDialog), builder);

  saveButton = (GtkButton *)gtk_builder_get_object (builder, "saveButton");
  g_signal_connect (saveButton, "clicked", G_CALLBACK (saveFileDialog), builder);

  helpButton = (GtkButton *)gtk_builder_get_object (builder, "helpButton");
  g_signal_connect (helpButton, "clicked", G_CALLBACK (loadHelpFile), NULL);
}

//Setup and name lefthand buttons from file.
void setUpPreviewButtons(GtkBuilder *builder) {
  int i = 0, filenameLength;
  GtkButton *button;
  char *filename;

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
//Functions for setUpPreviewButtons
GtkButton* setUpGtkButton(GtkBuilder *builder, char* buttonID, void (*function)(GtkButton*, gpointer)) {
  GtkButton* button;

  button = GTK_BUTTON(gtk_builder_get_object(builder, buttonID));
  g_signal_connect(button, "clicked", G_CALLBACK(function), buttonID);

  return button;
}

gchar* createButtonIDForIndex(int i) {
  char* buffer = NULL;

  if (i >= 0) {
    buffer = malloc(MAX_BUTTON_ID_LENGTH * sizeof *buffer);
    sprintf(buffer, "button%d", i);
  }

  return (gchar*)buffer;
}

void makeLowercase(char *filename) {

  int j = 0;

  while(filename[j] != '\0') {
      filename[j] = tolower(filename[j]);
      j++;
    }
}

char *extractFilenameFromPath(char* path) {
  char *token;
  char *temp = malloc(strlen(path) + 1);

  strcpy(temp, path);

  token = strchr(temp, '/');
  if (token == NULL) {
    fprintf(stderr, "Couldn't extract filename from path %s\n", path);
    return token;
  }
  token++;
  token = strtok(token, ".");

  free(temp);

  return token;
}

//-------------------------------------------------------

void launchTextEvent(void) {
  /*Seems a necceserry hack. It doesn't like passing sourcebuffer as arg to callback*/
  events_launchText(sourcebuffer);
}

//-------------------------------------------------------
//Open button needs to open a new window
void openFileDialog(GtkButton *button, GtkBuilder *builder) {
  GObject *window;
  GtkWidget *open_dialog;
  GtkFileChooser *chooser;
  GtkFileFilter *filter;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint result;

  UNUSED(button); //Stops the unused variable warning. Button is required for GTK even if not used
  filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter, "*.lump");
  gtk_file_filter_set_name(filter, "*.lump");
  //Get access to the main GUI window from the builder
  window = gtk_builder_get_object(builder, "window");
  //Set up the dialog window with open and cancel buttons
  open_dialog = gtk_file_chooser_dialog_new("Open File", (GtkWindow*) window,
                                             action, ("Cancel"), GTK_RESPONSE_CANCEL,
                                             ("Open"), GTK_RESPONSE_ACCEPT, NULL);
  chooser = GTK_FILE_CHOOSER (open_dialog);
  gtk_file_chooser_add_filter(chooser, filter);
  gtk_file_chooser_set_filter(chooser, filter);
  result = gtk_dialog_run(GTK_DIALOG (open_dialog));
//If the user presses the open button in the dialog box
  if (result == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = (char*)gtk_file_chooser_get_filename(chooser);
    //send the filename and sourcebuffer to the events module to read in the file
    events_openFile(filename, sourcebuffer); 
    free(filename);
  }
  gtk_widget_destroy(open_dialog);
}

//-------------------------------------------------------

void saveFileDialog(GtkButton *button, GtkBuilder *builder) {
  GObject *window;
  GtkWidget *save_dialog;
  GtkFileChooser *chooser;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint result;

  UNUSED(button);
  window = gtk_builder_get_object(builder, "window");
  //Set up the dialog window with save and cancel buttons
  save_dialog = gtk_file_chooser_dialog_new("Save File", (GtkWindow*) window,
                                           action, ("Cancel"), GTK_RESPONSE_CANCEL,
                                          ("_Save"), GTK_RESPONSE_ACCEPT, NULL);

  chooser = GTK_FILE_CHOOSER(save_dialog);
  gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
  //Suggest a name for the saved file
  gtk_file_chooser_set_current_name(chooser,("mytune.lump"));
  result = gtk_dialog_run(GTK_DIALOG(save_dialog));
  //If the user presses the save button in the dialog box
  if (result == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = (char*)gtk_file_chooser_get_filename(chooser);
    //send the filename and the sourcebuffer to the events module
    events_saveFile(filename, sourcebuffer); 
    free(filename);
  }
  gtk_widget_destroy(save_dialog);
}

void loadHelpFile(void) {
  //Load the helpfile by sending the filepath and the sourcebuffer to events
  char *filename = "documentation/helpFile.txt";
  events_openFile(filename, sourcebuffer);
}

//-------------------------------------------------------

//Load CSS file
void style(void) {
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
  GError *error = 0;
  gsize bytes_written, bytes_read;
  const gchar *style = "graphicsFiles/style.css";

  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gtk_css_provider_load_from_path (provider, g_filename_to_utf8(style, strlen(style), &bytes_read, &bytes_written, &error), NULL);
}

//-------------------------------------------------------

//Quit gtk_main loop
void closeGraphics(void) {
  gtk_main_quit();
}
