all:
	gcc src/*.c src/gamemodes/*.c -Wall -o tetrism -lSDL2 -lSDL_image -lSDL2_ttf -lm