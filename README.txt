Make using the following commands:

For Linux:
  make linux  <- This works on my install of Ubuntu.
                 If it doesn't work for you please
                 add another entry in to the make file
                 rather than changing this one.

For MacOS:
  make mac <- This should work. If you haven’t got GTK installed yet, downloading
		Homebrew and then doing ‘brew install gtk+3’ in Terminal is the most
		pain-free way to go.

For Windows:
  make win <- As above. I doubt this will work. Please change it to make it work.
              There are also ifdefs in audio.c feel free to ad the windows version
              on to the end. We may need to add an ifdef to graphics.c for the gtk
              library. Feel free!

To run the program "make run" should work!
