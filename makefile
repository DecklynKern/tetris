all:
	gcc src/*.c -Wall -o tetrism -L/usr/local/include/SDL2 -lSDL2main -lSDL2 -lSDL_mixer -lSDL_image

optimized:
	gcc src/*.c -Wall -O2 -o tetrism -L/usr/local/include/SDL2 -lSDL2main -lSDL2 -lSDL_mixer -lSDL_image