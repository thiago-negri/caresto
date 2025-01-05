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
# TILE ATLAS
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

    echo "#define GEN_TILE_ATLAS_PATH \"$TILE_ATLAS_PNG\"" >> $gen_file

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


#
# SPRITE ATLAS
#
need_sprite_atlas() {
    if [ ! -e "$SPRITE_ATLAS_PNG" ]; then
        return 0
    fi
    if [ ! -e "$SPRITE_ATLAS_H" ]; then
        return 0
    fi
    for file in ./$SPRITE_ATLAS_PATH/*.aseprite; do
        if [[ "$SPRITE_ATLAS_PNG" -ot "$file" ]]; then
            return 0
        fi
        if [[ "$SPRITE_ATLAS_H" -ot "$file" ]]; then
            return 0
        fi
    done
    return 1
}

generate_sprite_atlas() {
    echo "# convert $SPRITE_ATLAS_PATH/*.aseprite ..."

    aseprite -b \
        --ignore-layer 'bounding_box' \
        --data "$SPRITE_ATLAS_JSON" \
        --sheet "$SPRITE_ATLAS_PNG" \
        --sheet-type packed \
        --filename-format '{title}_{frame}' \
        --tagname-format '{title}!{tag}' \
        --list-tags \
        "$SPRITE_ATLAS_PATH/*.aseprite"

    aseprite -b \
        --all-layers \
        --layer 'bounding_box' \
        --trim \
        --ignore-empty \
        --merge-duplicates \
        --data "$SPRITE_ATLAS_BOUNDING_BOX_JSON" \
        --sheet-type packed \
        --filename-format '{title}' \
        "$SPRITE_ATLAS_PATH/*.aseprite"

    local sprite_names=($(jq \
        -r -c '.frames | keys | .[]' "$SPRITE_ATLAS_BOUNDING_BOX_JSON" | tr -d '\r' | sort -V))
    local frame_names=($(jq \
        -r -c '.frames | keys | .[]' "$SPRITE_ATLAS_JSON" | tr -d '\r' | sort -V))
    local animation_names=($(jq \
        -r -c '.meta.frameTags | map(.name) | .[]' "$SPRITE_ATLAS_JSON" | tr -d '\r' | sort -V))

    local gen_file=$SPRITE_ATLAS_H
    local module=$(basename $gen_file .h)

    echo "// This file is generated. Do not change it." > $gen_file
    echo "#ifndef ${module^^}_H" >> $gen_file
    echo "#define ${module^^}_H" >> $gen_file
    echo "" >> $gen_file
    echo "#define GEN_SPRITE_ATLAS_PATH \"$SPRITE_ATLAS_PNG\"" >> $gen_file
    echo "" >> $gen_file

    echo "enum gen_sprite_index {" >> $gen_file
    for sprite_name in "${sprite_names[@]}"; do
        echo "    GEN_SPRITE_${sprite_name^^}," >> $gen_file
    done
    echo "};" >> $gen_file
    echo "" >> $gen_file

    echo "enum gen_frame_index {" >> $gen_file
    for frame_name in "${frame_names[@]}"; do
        echo "    GEN_FRAME_${frame_name^^}," >> $gen_file
    done
    echo "};" >> $gen_file
    echo "" >> $gen_file

    echo "enum gen_animation_index {" >> $gen_file
    for animation_name in "${animation_names[@]}"; do
        local fix_animation_name="${animation_name//!/_}"
        echo "    GEN_ANIMATION_${fix_animation_name^^}," >> $gen_file
    done
    echo "};" >> $gen_file
    echo "" >> $gen_file

    echo "struct gen_frame { int u, v, w, h; };" >> $gen_file
    echo "const struct gen_frame gen_frame_atlas[] = {" >> $gen_file
    for frame_name in "${frame_names[@]}"; do
        local u=$(jq -r -c ".frames.\"${frame_name}\".frame.x" "$SPRITE_ATLAS_JSON")
        local v=$(jq -r -c ".frames.\"${frame_name}\".frame.y" "$SPRITE_ATLAS_JSON")
        local w=$(jq -r -c ".frames.\"${frame_name}\".frame.w" "$SPRITE_ATLAS_JSON")
        local h=$(jq -r -c ".frames.\"${frame_name}\".frame.h" "$SPRITE_ATLAS_JSON")

        echo "    [GEN_FRAME_${frame_name^^}] = {$u, $v, $w, $h}," >> $gen_file
    done
    echo "};" >> $gen_file
    echo "" >> $gen_file

    echo "struct gen_bounding_box { int x, y, w, h; };" >> $gen_file
    echo "const struct gen_bounding_box gen_bounding_box_atlas[] = {" >> $gen_file
    for sprite_name in "${sprite_names[@]}"; do
        local x=$(jq -r ".frames.\"${sprite_name}\".spriteSourceSize.x" \
            "$SPRITE_ATLAS_BOUNDING_BOX_JSON")

        local y=$(jq -r ".frames.\"${sprite_name}\".spriteSourceSize.y" \
            "$SPRITE_ATLAS_BOUNDING_BOX_JSON")

        local w=$(jq -r ".frames.\"${sprite_name}\".spriteSourceSize.w" \
            "$SPRITE_ATLAS_BOUNDING_BOX_JSON")

        local h=$(jq -r ".frames.\"${sprite_name}\".spriteSourceSize.h" \
            "$SPRITE_ATLAS_BOUNDING_BOX_JSON")

        echo "    [GEN_SPRITE_${sprite_name^^}] = {$x, $y, $w, $h}," >> $gen_file
    done
    echo "};" >> $gen_file
    echo "" >> $gen_file

    echo "struct gen_animation { enum gen_frame_index from, to; };" >> $gen_file
    echo "const struct gen_animation gen_animation_atlas[] = {" >> $gen_file
    for animation_name in "${animation_names[@]}"; do
        local sprite_name=$(echo "$animation_name" | awk -F'!' '{print $1}')
        local fix_animation_name="${animation_name//!/_}"

        local from=$(jq -r \
            ".meta.frameTags.[] | select(.name == \"${animation_name}\").from" \
            "$SPRITE_ATLAS_JSON")

        local to=$(jq -r \
            ".meta.frameTags.[] | select(.name == \"${animation_name}\").to" \
            "$SPRITE_ATLAS_JSON")

        echo -n "    [GEN_ANIMATION_${fix_animation_name^^}] =" >> $gen_file
        echo -n " {GEN_FRAME_${sprite_name^^}_$from," >> $gen_file
        echo " GEN_FRAME_${sprite_name^^}_$to}," >> $gen_file
    done
    echo "};" >> $gen_file
    echo "" >> $gen_file

    echo "#endif // ${module^^}_H" >> $gen_file
}
