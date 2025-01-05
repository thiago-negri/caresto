#!/bin/bash

#
# GLSL
#
need_generate_glsl() {
    local gen_file=$1
    local glsl_source=$2

    if [[ ! -e "$gen_file" || "$gen_file" -ot "$glsl_source" ]]; then
        return 0
    fi
    return 1
}

generate_glsl() {
    local gen_file=$1
    local glsl_source=$2

    local module=$(basename $gen_file .h)
    local timestamp=$(date +%s)

    echo "# convert $glsl_source to $gen_file"

    echo "// This file is generated. Do not change it." > $gen_file
    echo "#ifndef ${module^^}_H" >> $gen_file
    echo "#define ${module^^}_H" >> $gen_file

    echo "#ifdef SHARED" >> $gen_file
    echo "const long long ${module}_timestamp = $timestamp;" >> $gen_file
    echo "#endif // SHARED" >> $gen_file

    echo "const char *${module}_source =" >> $gen_file

    sed -E 's/(.*)/"\1\\n"/' $glsl_source >> $gen_file

    echo ";" >> $gen_file
    echo "#endif // ${module^^}_H" >> $gen_file
}


#
# ATLAS
#
need_tile_atlas() {
    if [ ! -e "$TILE_ATLAS_PNG" ]; then
        return 0
    fi
    if [ ! -e "$TILE_ATLAS_H" ]; then
        return 0
    fi
    if [[ "$TILE_ATLAS_PNG" -ot "$TILE_ATLAS_ASEPRITE" ]]; then
        return 0
    fi
    if [[ "$TILE_ATLAS_H" -ot "$TILE_ATLAS_ASEPRITE" ]]; then
        return 0
    fi
    return 1
}

generate_tile_atlas() {
    echo "# convert $TILE_ATLAS_ASEPRITE ..."

    aseprite -b "$TILE_ATLAS_ASEPRITE" \
        --data "$TILE_ATLAS_JSON" \
        --sheet "$TILE_ATLAS_PNG" \
        --sheet-type packed \
        --filename-format '{tag}'

    local gen_file=$TILE_ATLAS_H
    local module=$(basename $gen_file .h)
    echo "// This file is generated. Do not change it." > $gen_file
    echo "#ifndef ${module^^}_H" >> $gen_file
    echo "#define ${module^^}_H" >> $gen_file

    local first_sprite=$(jq '.frames | keys | first' "$TILE_ATLAS_JSON" | \
        grep '"' | sed -E 's/.*("[^"]*").*/\1/')

    local tile_size=$(jq ".frames.${first_sprite}.sourceSize.w" "$TILE_ATLAS_JSON" | \
        sed -E 's/(.*)/(\1)/')

    echo "#define GEN_TILE_ATLAS_TILE_SIZE $tile_size" >> $gen_file
    local sprite_names=$(jq '.frames | keys' "$TILE_ATLAS_JSON" | \
        grep '"' | sed -E 's/.*"([^"]*)".*/\1/')

    for sprite_name in $sprite_names; do
        local u=$(jq ".frames.\"${sprite_name}\".frame.x" "$TILE_ATLAS_JSON" | \
            sed -E 's/(.*)/(\1)/')

        local v=$(jq ".frames.\"${sprite_name}\".frame.y" "$TILE_ATLAS_JSON" | \
            sed -E 's/(.*)/(\1)/')

        echo "#define GEN_TILE_ATLAS_${sprite_name^^}_U $u" >> $gen_file
        echo "#define GEN_TILE_ATLAS_${sprite_name^^}_V $v" >> $gen_file
    done
    echo "#endif // ${module^^}_H" >> $gen_file
}
