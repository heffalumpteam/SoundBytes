NAME = test

CFILES = main.c graphics.c audio.c events.c

CFLAGS = -Wall -Wextra -Wfloat-equal -pedantic -std=c99

OFLAGS = -O3

linuxLIBS = -lm -lSDL2 -lSDL2_mixer

macFRAMEWORKS = -F/Library/Frameworks -framework SDL2 -framework SDL2_mixer
macINCLUDES = -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_mixer.framework/Headers

linuxGTK = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
macGTK = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
winGTK = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

$(NAME) :

linux : $(CFILES)
	gcc $(CFILES) -o $(NAME) $(CFLAGS) $(linuxGTK) $(linuxLIBS)

mac : $(CFILES)
	gcc $(CFILES) -o $(NAME) $(CFLAGS) $(macGTK) $(macFRAMEWORKS) $(macINCLUDES)

run: $(NAME)
	./$(NAME)

clean:
	rm $(NAME)
