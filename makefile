SDL_WIN_LIB = #put win path here e.g. SDL_WIN_LIB = C:/SDL
DEFAULT_ARGS = src/*.c src/gamemodes/*.c -Iinclude -Wall
LIBS =  -lSDL2 -lSDL2_image -lSDL2_ttf -lm
OUTPUT_NAME = -o tetrism

WIN_ISDL2 = -I$(SDL_WIN_LIB)/SDL2/include
WIN_ISDL2_IMAGE = -I$(SDL_WIN_LIB)/SDL2_image/include
WIN_ISDL2_TTF = -I$(SDL_WIN_LIB)/SDL2_ttf/include

LINUX_ARGS = -O2
WINDOWS_ARGS = -O2 -DSDL_MAIN_HANDLED -Wl,-subsystem,windows -lmingw32 $(WIN_ISDL2) $(WIN_ISDL2_IMAGE) $(WIN_ISDL2_TTF) $(WIN_ISDL2)/SDL2 $(WIN_ISDL2_IMAGE)/SDL2 $(WIN_ISDL2_TTF)/SDL2 -L$(SDL_WIN_LIB)/SDL2/lib -L$(SDL_WIN_LIB)/SDL2_image/lib -L$(SDL_WIN_LIB)/SDL2_ttf/lib
WASM_ARGS = -Os -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s USE_FREETYPE=1 -s WASM_BIGINT -s ENVIRONMENT=web --preload-file font --closure 1

bin:
	gcc $(DEFAULT_ARGS) $(OUTPUT_NAME) $(LINUX_ARGS) $(LIBS)

windows:
	gcc $(DEFAULT_ARGS) $(OUTPUT_NAME) $(WINDOWS_ARGS) $(LIBS)

wasm:
	emcc $(DEFAULT_ARGS) $(OUTPUT_NAME).html $(WASM_ARGS)