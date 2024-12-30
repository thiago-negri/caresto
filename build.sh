#!/bin/bash

TARGET_PATH=build/bin
TARGET=$TARGET_PATH/main.exe
OBJ_PATH=build/obj
SRC_PATH=src
COMPILE_FLAGS_ARR=(
    "-Iinclude"
    "-Isrc"
)
COMPILE_FLAGS="${COMPILE_FLAGS_ARR[*]}"
LINK_FLAGS_ARR=(
    "-Llib/windows/SDL3/x64"
    "-Llib/windows/glew/x64"
    "-lSDL3"
    "-lglew32"
    "-lglu32"
    "-lopengl32"
    "-Xlinker /SUBSYSTEM:WINDOWS" # Remove this if you want to see stdout / stderr on console
)
LINK_FLAGS="${LINK_FLAGS_ARR[*]}"

run() {
    echo "$@"
    $@
}

extract_headers() {
    local file=$1
    grep -oE '^\s*#\s*include\s*["<]([^">]+)[">]' "$file" | sed -E 's/\s*#\s*include\s*[<"]([^">]+)[">]/\1/'
}

need_compile() {
    local source_file=$1
    local module=$(basename $source_file .c)
    local obj_file=$OBJ_PATH/$module.o

    if [ ! -e "$obj_file" ]; then
        return 0
    fi
    if [[ "$obj_file" -ot "$source_file" ]]; then
        return 0
    fi

    local headers=$(extract_headers $source_file)
    for header_file in $headers; do
        if [[ -e "src/$header_file" && "$obj_file" -ot "src/$header_file" ]]; then
            return 0
        fi
        if [[ -e "include/$header_file" && "$obj_file" -ot "include/$header_file" ]]; then
            return 0
        fi
    done

    return 1
}

need_link() {
    if [ ! -e "$TARGET" ]; then
        return 0
    fi
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
    [ -e "$target_file" ] || run cp $file $target_file
}


#
# ARGS
#
arg_clean=1
arg_build=1
arg_run=1
for arg in "$@"; do
    case "$arg" in
        clean) arg_clean=0 ;;
        build) arg_build=0 ;;
        run) arg_run=0 ;;
    esac
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
[ $arg_clean -eq 0 ] && run rm -rf build


#
# BUILD
#
if [ $arg_build -eq 0 ]; then
    # PREPARE
    run mkdir -p "$OBJ_PATH" "$TARGET_PATH"

    # COMPILE
    for source_file in ./$SRC_PATH/*.c; do
        if need_compile "$source_file"; then
            module=$(basename $source_file .c)
            obj_file=$OBJ_PATH/$module.o
            run clang -c -o "$obj_file" "$source_file" $COMPILE_FLAGS
        fi
    done

    # LINK
    if need_link; then
        run clang -o "$TARGET" "$OBJ_PATH/*.o" $LINK_FLAGS
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
if [ $arg_run -eq 0 ]; then
    run ./$TARGET
    echo "$?"
fi

