#!/bin/bash

rm -rf include lib tmp-dependencies

mkdir tmp-dependencies
mkdir licenses
mkdir -p include/SDL3 lib/windows/SDL3
mkdir -p include/GL include/KHR
mkdir -p lib/windows/glew/x64 lib/windows/glew/x86

cd tmp-dependencies


#
# SDL
#
SDL=SDL3-devel-3.1.6-VC.zip
SDL_URL=https://github.com/libsdl-org/SDL/releases/download/preview-3.1.6/$SDL
SDL_FOLDER=SDL3-3.1.6
wget $SDL_URL
unzip $SDL
cp -r $SDL_FOLDER/include/* ../include/
cp -r $SDL_FOLDER/lib/* ../lib/windows/SDL3/
cp $SDL_FOLDER/README-SDL.txt ../licenses/README-SDL.txt


#
# GLEW
#
GLEW=glew-2.2.0-win32.zip
GLEW_URL=https://github.com/nigels-com/glew/releases/download/glew-2.2.0/$GLEW
GLEW_FOLDER=glew-2.2.0
wget $GLEW_URL
unzip $GLEW
cp -r $GLEW_FOLDER/include/GL/* ../include/GL/
cp -r $GLEW_FOLDER/lib/Release/x64/* ../lib/windows/glew/x64/
cp -r $GLEW_FOLDER/bin/Release/x64/glew32.dll ../lib/windows/glew/x64/glew32.dll
cp -r $GLEW_FOLDER/lib/Release/Win32/* ../lib/windows/glew/x86/
cp -r $GLEW_FOLDER/bin/Release/Win32/glew32.dll ../lib/windows/glew/x86/glew32.dll
cp $GLEW_FOLDER/LICENSE.txt ../licenses/LICENSE-glew.txt


#
# OpenGL
#
wget https://registry.khronos.org/OpenGL/api/GL/glext.h
wget https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
wget https://registry.khronos.org/OpenGL/api/GL/glxext.h
wget https://registry.khronos.org/OpenGL/api/GL/wglext.h
wget https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h
cp glext.h ../include/GL/glext.h
cp glcorearb.h ../include/GL/glcorearb.h
cp glxext.h ../include/GL/glxext.h
cp wglext.h ../include/GL/wglext.h
cp khrplatform.h ../include/KHR/khrplatform.h


#
# DONE
#
cd ..
rm -rf tmp-dependencies

