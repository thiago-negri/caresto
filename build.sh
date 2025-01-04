#!/bin/bash

#
# ARGS
#
arg_clean=1
arg_release=1
arg_build=1
arg_run=1
for arg in "$@"; do
    case "$arg" in
        clean) arg_clean=0 ;;
        release) arg_release=0 ;;
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
TARGET_SHARED=$TARGET_PATH/caresto.dll
TARGET=$TARGET_PATH/caresto.exe

OBJ_PATH=$BUILD_PATH/obj
OBJ_ENGINE_PATH=$BUILD_PATH/obj/engine
OBJ_CARESTO_PATH=$BUILD_PATH/obj/caresto

SRC_PATH=src
SRC_ENGINE_PATH=src/engine
SRC_CARESTO_PATH=src/caresto

GLSL_PATH=glsl

GEN_PATH=src-gen

INCLUDE_PATH=include

if [ $arg_release -eq 0 ]; then
    mapfile COMPILE_FLAGS_ARR < compile_flags_release.txt
else
    mapfile COMPILE_FLAGS_ARR < compile_flags.txt
fi

LINK_FLAGS_ARR=(
    "-Llib/windows/SDL3/x64"
    "-Llib/windows/glew/x64"
    "-lSDL3"
    "-lglew32"
    "-lglu32"
    "-lopengl32"
)

OBJ_FILES_TO_LINK_ARR=(
    "$OBJ_PATH/*.o"
    "$OBJ_ENGINE_PATH/*.o"
)

# Use 'release' option to:
# - Create an executable that doesn't spawn a console
# - -O3
# - Static link to game
if [ $arg_release -eq 0 ]; then
    LINK_FLAGS_ARR+=("-Xlinker /SUBSYSTEM:WINDOWS")
    BUILD_TYPE_FLAGS="-O3"
    OBJ_FILES_TO_LINK_ARR+=("$OBJ_CARESTO_PATH/*.o")
else
    LINK_FLAGS_ARR+=("-Xlinker /SUBSYSTEM:CONSOLE")
    BUILD_TYPE_FLAGS="-fsanitize=address -g -DDEBUG -DSHARED"
fi

LINK_FLAGS="${LINK_FLAGS_ARR[*]}"
OBJ_FILES_TO_LINK="${OBJ_FILES_TO_LINK_ARR[*]}"
COMPILE_FLAGS="${COMPILE_FLAGS_ARR[*]}"


#
# UTILS
#
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
    local local_obj_path=$2
    local module=$(basename $source_file .c)
    local obj_file=$local_obj_path/$module.o

    if [ ! -e "$obj_file" ]; then
        return 0
    fi
    if [[ "$obj_file" -ot "$source_file" ]]; then
        return 0
    fi

    local headers=$(extract_headers $source_file)
    for header_file in $headers; do
        if [[ -e "$SRC_PATH/$header_file" && "$obj_file" -ot "$SRC_PATH/$header_file" ]]; then
            return 0
        fi
        if [[ -e "$GEN_PATH/$header_file" && "$obj_file" -ot "$GEN_PATH/$header_file" ]]; then
            return 0
        fi
        if [[ -e "$INCLUDE_PATH/$header_file" && "$obj_file" -ot "$INCLUDE_PATH/$header_file" ]]; then
            return 0
        fi
    done

    return 1
}

need_shared() {
    # Never need shared library in release mode
    if [ $arg_release -eq 0 ]; then
        return 1
    fi

    if [ ! -e "$TARGET_SHARED" ]; then
        return 0
    fi

    for obj_file in ./$OBJ_ENGINE_PATH/*.o; do
        if [[ "$TARGET" -ot "$obj_file" ]]; then
            return 0
        fi
    done

    for obj_file in ./$OBJ_CARESTO_PATH/*.o; do
        if [[ "$TARGET" -ot "$obj_file" ]]; then
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

    for obj_file in ./$OBJ_ENGINE_PATH/*.o; do
        if [[ "$TARGET" -ot "$obj_file" ]]; then
            return 0
        fi
    done

    # Only link with static game objects if in release mode
    if [ $arg_release -eq 0 ]; then
        for obj_file in ./$OBJ_CARESTO_PATH/*.o; do
            if [[ "$TARGET" -ot "$obj_file" ]]; then
                return 0
            fi
        done
    fi

    return 1
}

static() {
    file=$1
    filename=$(basename $file)
    target_file=$TARGET_PATH/$filename
    [ -e "$target_file" ] || run cp $file $target_file
}


#
# CLEAN
#
[ $arg_clean -eq 0 ] && run rm -rf $BUILD_ROOT_PATH $GEN_PATH


#
# BUILD
#
if [ $arg_build -eq 0 ]; then
    # PREPARE
    run mkdir -p "$OBJ_PATH" "$OBJ_CARESTO_PATH" "$OBJ_ENGINE_PATH" "$TARGET_PATH" "$GEN_PATH/gen"

    # GENERATE FILES
    for glsl_source in $GLSL_PATH/*.glsl; do
        module=$(basename $glsl_source .glsl)
        gen_file=$GEN_PATH/gen/glsl_$module.h
        if [[ ! -e "$gen_file" || "$gen_file" -ot "$glsl_source" ]]; then
            echo "# convert $glsl_source to $gen_file"
            echo "// This file is generated. Do not change it." > $gen_file
            echo "#ifndef _GLSL_${module}_H" >> $gen_file
            echo "#define _GLSL_${module}_H" >> $gen_file
            echo "const long long glsl_${module}_timestamp =" >> $gen_file
            date +%s >> $gen_file
            echo ";" >> $gen_file
            echo "const char *glsl_${module}_source =" >> $gen_file
            sed -E 's/(.*)/"\1\\n"/' $glsl_source >> $gen_file
            echo ";" >> $gen_file
            echo "#endif // _GLSL_${module}_H" >> $gen_file
        fi
    done

    # COMPILE
    for source_file in ./$SRC_ENGINE_PATH/*.c; do
        if need_compile "$source_file" "$OBJ_ENGINE_PATH"; then
            module=$(basename $source_file .c)
            obj_file=$OBJ_ENGINE_PATH/$module.o
            run clang -c -o "$obj_file" "$source_file" $COMPILE_FLAGS $BUILD_TYPE_FLAGS
        fi
    done
    for source_file in ./$SRC_CARESTO_PATH/*.c; do
        if need_compile "$source_file" "$OBJ_CARESTO_PATH"; then
            module=$(basename $source_file .c)
            obj_file=$OBJ_CARESTO_PATH/$module.o
            run clang -c -o "$obj_file" "$source_file" $COMPILE_FLAGS $BUILD_TYPE_FLAGS
        fi
    done
    for source_file in ./$SRC_PATH/*.c; do
        if need_compile "$source_file" "$OBJ_PATH"; then
            module=$(basename $source_file .c)
            obj_file=$OBJ_PATH/$module.o
            run clang -c -o "$obj_file" "$source_file" $COMPILE_FLAGS $BUILD_TYPE_FLAGS
        fi
    done

    # SHARED LIBRARY
    if need_shared; then
        run clang -shared -o "$TARGET_SHARED" \
            "$OBJ_CARESTO_PATH/*.o" "$OBJ_ENGINE_PATH/*.o" $LINK_FLAGS $BUILD_TYPE_FLAGS
    fi

    # LINK
    if need_link; then
        run clang -o "$TARGET" $OBJ_FILES_TO_LINK $LINK_FLAGS $BUILD_TYPE_FLAGS
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

