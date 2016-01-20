NAME = test

CFILES = main.c graphics.c audio.c events.c text.c

CFLAGS = -Wall -Wextra -Wfloat-equal -pedantic -std=c99

NO_WARN_UNUSED = -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

OFLAGS = -O3

linuxLIBS = -lm -lSDL2 -lSDL2_mixer

macFRAMEWORKS = -F/Library/Frameworks -framework SDL2 -framework SDL2_mixer
macINCLUDES = -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_mixer.framework/Headers -I/usr/local/include/gtksourceview-3.0
winINCLUDES = -lm -lmingw32 -lSDL2main -I /c/mingw64/include -L /c/mingw64/lib -lSDL2 -mwindows -lSDL2_image -lSDL2_mixer -I /c/msys64/mingw64/include/gtksourceview-3.0

linuxGTK = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0 gtksourceview-3.0`
macGTK = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0 gtksourceview-3.0`
winGTK = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0 gtksourceview-3.0`

$(NAME) :

linux : $(CFILES)
	gcc $(CFILES) -o $(NAME) $(OFLAGS) -fopenmp $(CFLAGS) $(linuxGTK) $(linuxLIBS)

mac : $(CFILES)
	gcc-5 $(CFILES) -o $(NAME) $(OFLAGS) $(CFLAGS) -fopenmp $(macGTK) $(macFRAMEWORKS) $(macINCLUDES)

win : $(CFILES)
	gcc $(CFILES) -o $(NAME).exe $(OFLAGS) $(CFLAGS) -fopenmp $(winINCLUDES) $(winGTK)



run: $(NAME)
	./$(NAME)

clean:
	rm $(NAME)

debug:
		gcc $(CFILES) -o $(NAME) $(CFLAGS) -g $(linuxGTK) $(linuxLIBS) $(NO_WARN_UNUSED)

linuxtest: $(CFILES)
	gcc tests.c -o tests $(CFLAGS) -g $(linuxGTK) $(linuxLIBS) -fopenmp
	./tests 2> testerrors.txt

mactest: $(CFILES)
	gcc-5 tests.c -o tests $(CFLAGS) -g $(macINCLUDES) $(macGTK) $(macFRAMEWORKS) -fopenmp
	./tests 2> testerrors.txt

wintest: $(CFILES)
	gcc tests.c -o tests.exe $(CFLAGS) -fopenmp $(winINCLUDES) $(winGTK)

rundgb:
	gdb $(NAME)
