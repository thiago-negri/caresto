#!/bin/bash

source ./sh/vars.sh

source ./sh/functions.sh

source ./sh/generators.sh


#
# CLEAN
#
[ $arg_clean -eq 0 ] && run "# clean directories ..." rm -rf $BUILD_ROOT_PATH $GEN_PATH


# PREPARE
mkdir -p "$OBJ_PATH" "$OBJ_GEN_PATH" \
    "$TARGET_PATH" "$GEN_PATH/gen"


#
# GENERATE
#
if [ $arg_generate -eq 0 ]; then
    # GENERATE FILES
    for glsl_source in $(find $GLSL_PATH/ -type f -name '*.glsl'); do
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

    # GENERATE SPRITE ATLAS
    if need_sprite_atlas; then
        generate_sprite_atlas
    fi

    # GENERATE STRINGS
    if need_strings; then
        generate_strings
    fi
fi


#
# BUILD
#
if [ $arg_build -eq 0 ]; then
    # COMPILE
    for source_file in $(find src/ -mindepth 2 -type f -name '*.c'); do
        sub_obj_path=$(dirname $source_file | cut -f 2- -d '/')
        if need_compile "$OBJ_PATH/$sub_obj_path" "$source_file"; then
            mkdir -p "$OBJ_PATH/$sub_obj_path"
            compile "$OBJ_PATH/$sub_obj_path" "$source_file"
        fi
    done

    for source_file in $(find $GEN_PATH/gen/ -type f -name '*.c'); do
        if need_compile "$OBJ_GEN_PATH" "$source_file"; then
            compile "$OBJ_GEN_PATH" "$source_file"
        fi
    done

    for source_file in $(find $SRC_PATH/ -maxdepth 1 -type f -name '*.c'); do
        if need_compile "$OBJ_PATH" "$source_file"; then
            compile "$OBJ_PATH" "$source_file"
        fi
    done

    # SHARED LIBRARY
    if need_shared; then
        run "# create shared $TARGET_SHARED ..." clang -shared -o "$TARGET_SHARED" \
            $(find $OBJ_PATH -mindepth 2 -type f -name '*.o') \
            $LINK_FLAGS $BUILD_TYPE_FLAGS
    fi

    # LINK
    if need_link; then
        if [ $arg_release -eq 0 ]; then
            obj_files=$(find $OBJ_PATH -type f -name '*.o')
        else
            obj_files=$(find $OBJ_PATH -type f -name '*.o' ! -wholename '*/caresto/*')
        fi
        run "# link $TARGET ..." clang -o "$TARGET" $obj_files $LINK_FLAGS $BUILD_TYPE_FLAGS
    fi

    # COPY DLLS AND LICENSES
    if [ "$os" == "win" ]; then
        static lib/windows/SDL3/x64/SDL3.dll
        static lib/windows/SDL3_ttf/x64/SDL3_ttf.dll
        static lib/windows/glew/x64/glew32.dll
    fi
    static licenses/README-SDL.txt
    static licenses/LICENSE-glew.txt
    static licenses/LICENSE-freetype.txt
    static licenses/Montserrat-OFL.txt
fi


#
# RUN
#
if [ $arg_run -eq 0 ]; then
    run "# executing ..." ./$TARGET
    echo "$?"
fi

