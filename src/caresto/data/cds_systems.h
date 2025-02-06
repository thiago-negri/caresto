#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <caresto/opengl/coo_opengl.h>
#include <gen/sprite_atlas.h>
#include <gen/strings.h>

#define CDS_DEBUG_MAX 100
#define CDS_BODIES_MAX 100
#define CDS_SPRITES_MAX 100
#define CDS_ANIMATIONS_MAX 100
#define CDS_TILES_MAX 1000
#define CDS_UI_TEXTS_MAX 100

#define CDS_TILE_MAP_MAX_WIDTH 100
#define CDS_TILE_MAP_MAX_HEIGHT 100

#define CDS_SPRITE_MIRROR_X 1

typedef unsigned char cds_ui_text_id;

struct cds_ui_text {
    int x, y;
    GLuint texture_id;
};

struct cds_ui {
    TTF_Font *font;
    unsigned char text_count;
    unsigned char text_ids[CDS_UI_TEXTS_MAX];
    struct cds_ui_text texts[CDS_UI_TEXTS_MAX];
    struct coo_sprite texts_gpu[CDS_UI_TEXTS_MAX];
};

struct cds_camera {
    float x, y, w, h;
};

struct cds_debug {
    unsigned int debug_count;
    struct coo_debug debug_gpu[CDS_DEBUG_MAX];
};

typedef unsigned char cds_sprite_id;

struct cds_sprite {
    int x, y;
    int w, h;
    int u, v;
    unsigned int flags;
};

struct cds_sprite_map {
    unsigned char sprite_count;
    unsigned char ids[CDS_SPRITES_MAX];
    struct cds_sprite sprites[CDS_SPRITES_MAX];
    struct coo_sprite sprites_gpu[CDS_SPRITES_MAX];
};

typedef unsigned char cds_animation_id;

struct cds_animation {
    cds_sprite_id sprite;
    enum gen_animation_index animation_index;
    unsigned char current_frame;
    double duration_remaining;
};

struct cds_animation_map {
    unsigned char animation_count;
    unsigned char ids[CDS_ANIMATIONS_MAX];
    struct cds_animation animations[CDS_ANIMATIONS_MAX];
};

typedef unsigned char cds_body_id;

struct cds_body {
    struct em_ipos position;
    struct em_isize size;
    struct em_vec2 velocity;
    struct em_vec2 movement_remaining;
};

struct cds_body_map {
    unsigned char body_count;
    unsigned char ids[CDS_BODIES_MAX];
    struct cds_body bodies[CDS_BODIES_MAX];
};

enum cds_entity_type {
    cds_entity_undefined,
    cds_entity_beetle,
    cds_entity_carestosan,
};

struct cds_beetle {
    enum cds_entity_type type;
    cds_sprite_id sprite;
    cds_body_id body;
    cds_animation_id animation;
    struct em_ivec2 position;
};

struct cds_carestosan {
    enum cds_entity_type type;
    cds_sprite_id sprite;
    cds_body_id body;
    cds_animation_id animation;
    struct em_ivec2 position;

    cds_ui_text_id texts[4];
};

union cds_entity {
    enum cds_entity_type type;
    struct cds_beetle beetle;
    struct cds_carestosan carestosan;
};

enum cds_tile_type : unsigned char {
    CDS_EMPTY,
    CDS_SOLID,
};

struct cds_tile_map {
    int tile_count;
    enum cds_tile_type tile_map[CDS_TILE_MAP_MAX_HEIGHT]
                               [CDS_TILE_MAP_MAX_WIDTH];
    struct coo_sprite tiles_gpu[CDS_TILES_MAX];
};

struct cds_systems {
    enum gen_string_lang lang;
    struct cds_camera camera;

#ifdef DEBUG
    bool debug_enabled;
    struct cds_debug debug;
#endif // DEBUG

    struct cds_carestosan carestosan;
    struct cds_beetle beetle;

    struct cds_animation_map animation_map;
    struct cds_body_map body_map;
    struct cds_sprite_map sprite_map;
    struct cds_tile_map tile_map;
    struct cds_ui ui;
};
