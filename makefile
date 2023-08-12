all:
	gcc src/*.c src/gamemodes/*.c -Wall -o tetrism -L/usr/local/include/SDL2 -lSDL2main -lSDL2 -lSDL_mixer -lSDL_image -lSDL2_ttf -lm