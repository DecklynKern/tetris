all:
	gcc src/*.c -Wall -o tetris -L/usr/local/include/SDL2 -lSDL2main -lSDL2 -lSDL_mixer -lSDL_image

optimized:
	gcc src/*.c -Wall -O2 -o tetris -L/usr/local/include/SDL2 -lSDL2main -lSDL2 -lSDL_mixer -lSDL_image