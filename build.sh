#!/bin/bash

TARGET_PATH=build/bin
TARGET=$TARGET_PATH/main.exe
OBJ_PATH=build/obj
SRC_PATH=src
COMPILE_FLAGS="-Iinclude -Isrc"
LINK_FLAGS="-Llib/windows/SDL3/x64 -Llib/windows/glew/x64 \
    -lSDL3 -lglew32 -lglu32 -lopengl32 \
    -Xlinker /SUBSYSTEM:WINDOWS"

run() {
    echo "$@"
    $@
}

need_link() {
    for obj_file in ./$OBJ_PATH/*.o; do
        if [[ "$TARGET" -ot "$obj_file" ]]; then
            return 0
        fi
    done
    return 1
}

static() {
    file=$1
    filename=$(basename $file)
    target_file=$TARGET_PATH/$filename
    [ -e $target_file ] || run cp $file $target_file
}


#
# ARGS
#
arg_clean=1
arg_build=1
arg_run=1
for arg in "$@"; do
    if [[ "$arg" == "clean" ]]; then
        arg_clean=0
    fi
    if [[ "$arg" == "build" ]]; then
        arg_build=0
    fi
    if [[ "$arg" == "run" ]]; then
        arg_run=0
    fi
done
# DEFAULTS (build run)
if [[ "$arg_clean$arg_build$arg_run" == "111" ]]; then
    arg_clean=1
    arg_build=0
    arg_run=0
fi

#
# CLEAN
#
[[ "$arg_clean" == "0" ]] && run rm -rf build


#
# BUILD
#
if [[ "$arg_build" == "0" ]]; then
    # PREPARE
    mkdir -p $OBJ_PATH $TARGET_PATH

    # COMPILE
    for source_file in ./$SRC_PATH/*.c; do
        module=$(basename $source_file .c)
        obj_file=$OBJ_PATH/$module.o
        if [[ "$obj_file" -ot "$source_file" ]]; then
            run clang -c -o $obj_file $source_file $COMPILE_FLAGS
        fi
    done

    # LINK
    if need_link; then
        run clang -o $TARGET $OBJ_PATH/*.o $LINK_FLAGS
    fi

    # COPY DLLS AND LICENSES
    static lib/windows/SDL3/x64/SDL3.dll
    static licenses/README-SDL.txt
    static lib/windows/glew/x64/glew32.dll
    static licenses/LICENSE-glew.txt
fi


#
# RUN
#
if [[ "$arg_run" == "0" ]]; then
    run ./$TARGET
    echo "$?"
fi

