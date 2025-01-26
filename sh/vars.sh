#!/bin/bash

unameOut="$(uname -s)"
case "${unameOut}" in
    Darwin*)    os=mac;;
    MINGW*)     os=win;;
    *)          os=linux;;
esac

#
# ARGS
#
arg_clean=1
arg_release=1
arg_generate=1
arg_build=1
arg_run=1
arg_verbose=1
for arg in "$@"; do
    case "$arg" in
        clean) arg_clean=0 ;;
        release) arg_release=0 ;;
        verbose) arg_verbose=0 ;;
        generate) arg_generate=0 ;;
        build) 
            arg_generate=0
            arg_build=0 
            ;;
        run) arg_run=0 ;;
    esac
done

# DEFAULTS (generate build run)
if [[ "$arg_clean$arg_generate$arg_build$arg_run" == "1111" ]]; then
    arg_clean=1
    arg_generate=0
    arg_build=0
    arg_run=0
fi


#
# VARS
#
if [ $arg_release -eq 0 ]; then
    BUILD_TYPE="release"
else
    BUILD_TYPE="debug"
fi

BUILD_ROOT_PATH=build
BUILD_PATH=$BUILD_ROOT_PATH/$BUILD_TYPE

TARGET_PATH=$BUILD_PATH/bin
if [ "$os" == "win" ]; then
    TARGET_SHARED=$TARGET_PATH/caresto.dll
    TARGET=$TARGET_PATH/caresto.exe
fi
if [ "$os" == "linux" ]; then
    TARGET_SHARED=$TARGET_PATH/caresto.so
    TARGET=$TARGET_PATH/caresto
fi

SRC_PATH=src
OBJ_PATH=$BUILD_PATH/obj

GLSL_PATH=glsl

GEN_PATH=src_gen
OBJ_GEN_PATH=$BUILD_PATH/obj/gen

INCLUDE_PATH=include

if [ $arg_release -eq 0 ]; then
    mapfile COMPILE_FLAGS_ARR < compile_flags_release.txt
else
    mapfile COMPILE_FLAGS_ARR < compile_flags.txt
fi

if [ "$os" == "win" ]; then
    LINK_FLAGS_ARR=(
        "-Llib/windows/SDL3/x64"
        "-Llib/windows/glew/x64"
        "-lglew32"
        "-lglu32"
        "-lopengl32"
    )
fi
if [ "$os" == "linux" ]; then
    LINK_FLAGS_ARR=(
        "-lSDL3"
        "-lGLEW"
        "-lGL"
        "-lX11"
        "-lGLU"
        "-lOpenGL"
        "-lpthread"
    )
fi

# Use 'release' option to:
# - Create an executable that doesn't spawn a console
# - -O3
# - Static link to game
# - Static link to SDL and GLEW
if [ $arg_release -eq 0 ]; then
    if [ "$os" == "win" ]; then
        LINK_FLAGS_ARR+=("-Xlinker /SUBSYSTEM:WINDOWS")
        # TODO(tnegri): Static link SDL and GLEW
    fi
    if [ "$os" == "linux" ]; then
        LINK_FLAGS_ARR+=("-lm")
        LINK_FLAGS_ARR+=("-Llib/linux/SDL3/x64")
        LINK_FLAGS_ARR+=("-Llib/linux/GLEW/x64")
    fi
    BUILD_TYPE_FLAGS="-O3"
else
    if [ "$os" == "win" ]; then
        LINK_FLAGS_ARR+=("-Xlinker /SUBSYSTEM:CONSOLE")
    fi
    BUILD_TYPE_FLAGS="-fsanitize=address -g -DDEBUG -DSHARED"
fi

LINK_FLAGS="${LINK_FLAGS_ARR[*]}"
COMPILE_FLAGS="${COMPILE_FLAGS_ARR[*]}"

TILE_ATLAS_ASEPRITE="assets/tiles/tiles.aseprite"
TILE_ATLAS_JSON="assets/tiles.json"
TILE_ATLAS_PNG="assets/tile_atlas.png"
TILE_ATLAS_H="$GEN_PATH/gen/tile_atlas.h"

SPRITE_ATLAS_PATH="assets/sprites"
SPRITE_ATLAS_JSON="assets/sprites.json"
SPRITE_ATLAS_BOUNDING_BOX_JSON="assets/sprites_bounding_box.json"
SPRITE_ATLAS_PNG="assets/sprite_atlas.png"
SPRITE_ATLAS_GEN_MODULE="$GEN_PATH/gen/sprite_atlas"

