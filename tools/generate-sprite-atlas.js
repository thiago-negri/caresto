import fs from 'fs';
import path from 'path';

function main() {
    if (process.argv.length < 5) {
        console.error('invalid use');
        return;
    }

    let i = 2;
    const atlasPngPath = process.argv[i++];
    const atlasJsonPath = process.argv[i++];
    const boundingBoxJsonPath = process.argv[i++];
    const module = process.argv[i++];

    const guard = `${path.basename(module).toUpperCase()}_H`;

    const atlas = JSON.parse(fs.readFileSync(atlasJsonPath));
    const boundingBox = JSON.parse(fs.readFileSync(boundingBoxJsonPath));

    const spriteNames = Object.keys(boundingBox.frames);
    const frameNames = Object.keys(atlas.frames);
    /** @type string[] */
    const animationNames = atlas.meta.frameTags.map((e) => e.name);

    /** @type string[] */
    const headerLines = [];
    headerLines.push("// This file is generated. Do not change it.");
    headerLines.push(`#ifndef ${guard}`);
    headerLines.push(`#define ${guard}`);
    headerLines.push("");
    headerLines.push(`#define GEN_SPRITE_ATLAS_PATH "${atlasPngPath}"`);
    headerLines.push("");
    headerLines.push(`#define GEN_FRAME_COUNT ${frameNames.length}`);
    headerLines.push(`#define GEN_SPRITE_COUNT ${spriteNames.length}`);
    headerLines.push(`#define GEN_ANIMATION_COUNT ${animationNames.length}`);
    headerLines.push("");
    headerLines.push("enum gen_sprite_index {");
    for (const spriteName of spriteNames) {
        headerLines.push(`    GEN_SPRITE_${spriteName.toUpperCase()},`);
    }
    headerLines.push("};");
    headerLines.push("");
    headerLines.push("enum gen_frame_index {");
    for (const frameName of frameNames) {
        headerLines.push(`    GEN_FRAME_${frameName.toUpperCase()},`);
    }
    headerLines.push("};");
    headerLines.push("");
    headerLines.push("enum gen_animation_index {");
    for (const animationName of animationNames) {
        const fixAnimationName = animationName.replaceAll('!', '_').toUpperCase();
        headerLines.push(`    GEN_ANIMATION_${fixAnimationName},`);
    }
    headerLines.push("};");
    headerLines.push("");
    headerLines.push("struct gen_frame { int u, v, w, h, duration; };");
    headerLines.push("extern const struct gen_frame gen_frame_atlas[GEN_FRAME_COUNT];");
    headerLines.push("");
    headerLines.push("struct gen_bounding_box { int x, y, w, h; };");
    headerLines.push("extern const struct gen_bounding_box gen_bounding_box_atlas[GEN_SPRITE_COUNT];");
    headerLines.push("");
    headerLines.push("struct gen_animation { enum gen_frame_index from, to; };");
    headerLines.push("extern const struct gen_animation gen_animation_atlas[GEN_ANIMATION_COUNT];");
    headerLines.push("");
    headerLines.push(`#endif // ${guard}`);

    /** @type string[] */
    const implLines = [];
    implLines.push("// This file is generated. Do not change it.");
    implLines.push("");
    implLines.push(`#include <gen/${path.basename(module)}.h>`);
    implLines.push("");
    implLines.push("const struct gen_frame gen_frame_atlas[GEN_FRAME_COUNT] = {");
    for (const frameName of frameNames) {
        const u = atlas.frames[frameName].frame.x;
        const v = atlas.frames[frameName].frame.y;
        const w = atlas.frames[frameName].frame.w;
        const h = atlas.frames[frameName].frame.h;
        const d = atlas.frames[frameName].duration;
        implLines.push(`    [GEN_FRAME_${frameName.toUpperCase()}] = {${u}, ${v}, ${w}, ${h}, ${d}},`);
    }
    implLines.push("};");
    implLines.push("");
    implLines.push("const struct gen_bounding_box gen_bounding_box_atlas[GEN_SPRITE_COUNT] = {");
    for (const spriteName of spriteNames) {
        const x = boundingBox.frames[spriteName].spriteSourceSize.x;
        const y = boundingBox.frames[spriteName].spriteSourceSize.y;
        const w = boundingBox.frames[spriteName].spriteSourceSize.w;
        const h = boundingBox.frames[spriteName].spriteSourceSize.h;
        implLines.push(`    [GEN_SPRITE_${spriteName.toUpperCase()}] = {${x}, ${y}, ${w}, ${h}},`);
    }
    implLines.push("};");
    implLines.push("");
    implLines.push("const struct gen_animation gen_animation_atlas[GEN_ANIMATION_COUNT] = {");
    for (const animationName of animationNames) {
        const spriteName = animationName.split('!')[0];
        const fixAnimationName = animationName.replaceAll('!', '_');
        const animation = atlas.meta.frameTags.find((e) => e.name == animationName);
        const { from, to } = animation;
        implLines.push(`    [GEN_ANIMATION_${fixAnimationName.toUpperCase()}] =` +
                       ` {GEN_FRAME_${spriteName.toUpperCase()}_${from},` +
                       ` GEN_FRAME_${spriteName.toUpperCase()}_${to}},`);
    }
    implLines.push("};");

    const genFileH = `${module}.h`;
    fs.writeFileSync(genFileH, headerLines.join('\n'));

    const genFileC = `${module}.c`;
    fs.writeFileSync(genFileC, implLines.join('\n'));
}

main();
