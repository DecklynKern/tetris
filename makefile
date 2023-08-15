pbin:
	gcc src/*.c src/gamemodes/*.c -Wall -lSDL2 -lSDL_image -lSDL2_ttf -lm -o tetrism

wasm:
	emcc src/*.c src/gamemodes/*.c -Wall -O3 -o tetrism.html -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2