#!/bin/bash

#
# UTILS
#
run() {
    name=$1
    shift

    if [ $arg_verbose -eq 0 ]; then
        echo "$@"
    else
        echo "$name"
    fi

    "$@"
}

extract_headers() {
    local file=$1
    grep -oE '^\s*#\s*include\s*["<]([^">]+)[">]' "$file" | \
        sed -E 's/\s*#\s*include\s*[<"]([^">]+)[">]/\1/'
}

need_compile() {
    local obj_path=$1
    local source_file=$2

    local module=$(basename $source_file .c)
    local obj_file=$obj_path/$module.o

    if [ ! -e "$obj_file" ]; then
        return 0
    fi

    if [[ "$obj_file" -ot "$source_file" ]]; then
        return 0
    fi

    local headers=$(extract_headers $source_file)
    for header_file in $headers; do
        local header_path=";"
        if [[ -e "$SRC_PATH/$header_file" ]]; then
            header_path="$SRC_PATH/$header_file"
        fi
        if [[ -e "$GEN_PATH/$header_file" ]]; then
            header_path="$GEN_PATH/$header_file"
        fi
        if [[ -e "$INCLUDE_PATH/$header_file" ]]; then
            header_path="$INCLUDE_PATH/$header_file"
        fi

        if [[ -e "$header_path" ]]; then
            if [[ "$obj_file" -ot "$SRC_PATH/$header_file" ]]; then
                return 0
            fi

            local headers_2=$(extract_headers $header_path)
            for header_file_2 in $headers_2; do
                if [[ -e "$SRC_PATH/$header_file_2" && "$obj_file" -ot "$SRC_PATH/$header_file_2" ]]; then
                    return 0
                fi
                if [[ -e "$GEN_PATH/$header_file_2" && "$obj_file" -ot "$GEN_PATH/$header_file_2" ]]; then
                    return 0
                fi
                if [[ -e "$INCLUDE_PATH/$header_file_2" && "$obj_file" -ot "$INCLUDE_PATH/$header_file_2" ]]; then
                    return 0
                fi
            done
        fi
    done

    return 1
}

compile() {
    local obj_path=$1
    local source_file=$2

    local module=$(basename $source_file .c)
    local obj_file=$obj_path/$module.o

    run "# compile $source_file ..." \
        clang -c -o "$obj_file" "$source_file" $COMPILE_FLAGS $BUILD_TYPE_FLAGS
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
        if [[ "$TARGET_SHARED" -ot "$obj_file" ]]; then
            return 0
        fi
    done

    for obj_file in ./$OBJ_CARESTO_PATH/*.o; do
        if [[ "$TARGET_SHARED" -ot "$obj_file" ]]; then
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
    [ -e "$target_file" ] || run "# copy $target_file ..." cp $file $target_file
}
