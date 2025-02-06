#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Darwin*)    os=mac;;
    MINGW*)     os=win;;
    *)          os=linux;;
esac

rm -rf include lib tmp-dependencies

mkdir tmp-dependencies
mkdir licenses
mkdir -p include/SDL3 lib/windows/SDL3
mkdir -p lib/windows/SDL3_ttf
mkdir -p include/GL include/KHR
mkdir -p lib/windows/glew/x64 lib/windows/glew/x86

cd tmp-dependencies


#
# SDL
#
if [ "$os" == "win" ]; then
    SDL=SDL3-devel-3.2.0-VC.zip
    SDL_URL=https://github.com/libsdl-org/SDL/releases/download/release-3.2.0/$SDL
    SDL_FOLDER=SDL3-3.2.0
    wget $SDL_URL
    unzip $SDL
    cp -r $SDL_FOLDER/include/* ../include/
    cp -r $SDL_FOLDER/lib/* ../lib/windows/SDL3/
    cp $SDL_FOLDER/README-SDL.txt ../licenses/README-SDL.txt
fi
if [ "$os" == "linux" ]; then
    sudo dnf install SDL3-devel
fi

#
# SDL_ttf
#
if [ "$os" == "win" ]; then
    SDL_TTF=SDL3_ttf-devel-3.1.0-VC.zip
    SDL_TTF_URL=https://github.com/libsdl-org/SDL_ttf/releases/download/preview-3.1.0/$SDL_TTF
    SDL_TTF_FOLDER=SDL3_ttf-3.1.0
    wget $SDL_TTF_URL
    unzip $SDL_TTF
    cp -r $SDL_TTF_FOLDER/include/* ../include/
    cp -r $SDL_TTF_FOLDER/lib/* ../lib/windows/SDL3_ttf/
    cp $SDL_TTF_FOLDER/LICENSE.txt ../licenses/LICENSE-SDL_ttf.txt
fi


#
# GLEW
#
if [ "$os" == "win" ]; then
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
fi
if [ "$os" == "linux" ]; then
    sudo dnf install glew-devel
fi


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
# STBI_IMAGE
#
wget https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h
cp stb_image.h ../include/stb_image.h


#
# DONE
#
cd ..
rm -rf tmp-dependencies

