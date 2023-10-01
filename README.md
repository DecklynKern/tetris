# tetrism
Font used: https://www.dafont.com/aldo-the-apache.font

# Build info
## Linux
Dependencies: gcc, SDL2/SDL2_ttf/SDL2_image

Run ```make```.

## Web
Command assumes build from Linux.

Same dependencies as Linux build but also requires [Emscripten](https://emscripten.org) to be installed and in the PATH.

Run ```make wasm```.

## Windows
Dependencies:
- CMake and gcc installed
- MinGW installed and in the PATH
- [SDL2](https://github.com/libsdl-org/SDL/releases), [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases), and [SDL2_image](https://github.com/libsdl-org/SDL_image/releases) all from "[package name]-devel-2.xx.x-mingw" all in one folder put into the makefile at SDL_WIN_LIB

Runtime requirements:
SDL2.dll and SDL2_ttf.dll from the bin folders of their respective downloads in the same folder as the executable.

Run ```make windows```.