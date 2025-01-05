#!/bin/bash

source ./sh/vars.sh

source ./sh/functions.sh

source ./sh/generators.sh


#
# CLEAN
#
[ $arg_clean -eq 0 ] && run rm -rf $BUILD_ROOT_PATH $GEN_PATH


#
# BUILD
#
if [ $arg_build -eq 0 ]; then
    # PREPARE
    run mkdir -p "$OBJ_PATH" "$OBJ_CARESTO_PATH" \
        "$OBJ_ENGINE_PATH" "$TARGET_PATH" "$GEN_PATH/gen"

    # GENERATE FILES
    for glsl_source in $GLSL_PATH/*.glsl; do
        module=$(basename $glsl_source .glsl)
        gen_file=$GEN_PATH/gen/glsl_$module.h
        if need_generate_glsl $gen_file $glsl_source; then
            generate_glsl $gen_file $glsl_source
        fi
    done

    # GENERATE TILE ATLAS
    if need_tile_atlas; then
        generate_tile_atlas
    fi

    # COMPILE
    for source_file in ./$SRC_ENGINE_PATH/*.c; do
        if need_compile "$OBJ_ENGINE_PATH" "$source_file"; then
            compile "$OBJ_ENGINE_PATH" "$source_file"
        fi
    done
    for source_file in ./$SRC_CARESTO_PATH/*.c; do
        if need_compile "$OBJ_CARESTO_PATH" "$source_file"; then
            compile "$OBJ_CARESTO_PATH" "$source_file"
        fi
    done
    for source_file in ./$SRC_PATH/*.c; do
        if need_compile "$OBJ_PATH" "$source_file"; then
            compile "$OBJ_PATH" "$source_file"
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

