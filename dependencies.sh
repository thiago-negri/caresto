#!/bin/bash

rm -rf include lib tmp-dependencies

mkdir -p include/SDL lib/windows/SDL licenses tmp-dependencies

cd tmp-dependencies

#
# SDL
#
SDL=SDL2-devel-2.30.10-VC.zip
SDL_URL=https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/$SDL
SDL_FOLDER=SDL2-2.30.10
wget $SDL_URL
unzip $SDL
cp -r $SDL_FOLDER/include/* ../include/SDL/
cp -r $SDL_FOLDER/lib/* ../lib/windows/SDL/
cp $SDL_FOLDER/README-SDL.txt ../licenses/README-SDL.txt

#
# DONE
#
cd ..
rm -rf tmp-dependencies

