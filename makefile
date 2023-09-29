bin:
	gcc src/*.c src/gamemodes/*.c -Iinclude -Wall -O2 -lSDL2 -lSDL_image -lSDL2_ttf -lm -o tetrism

wasm:
	emcc --closure 1 src/*.c src/gamemodes/*.c -Iinclude -Wall -Os -o tetrism.html -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s USE_FREETYPE=1 -s WASM_BIGINT -s ENVIRONMENT=web --preload-file font
