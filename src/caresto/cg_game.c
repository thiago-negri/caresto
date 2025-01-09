#include <SDL3/SDL.h>
#include <caresto/ca_animation.h>
#include <caresto/cb_bodymap.h>
#include <caresto/cc_camera.h>
#include <caresto/ce_entity.h>
#include <caresto/cg_game.h>
#include <caresto/cgl_opengl.h>
#include <caresto/cs_spritemap.h>
#include <caresto/ct_tilemap.h>
#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/et_test.h>
#include <engine/eu_utils.h>
#include <gen/sprite_atlas.h>
#include <gen/tile_atlas.h>

// #define GAME_CAMERA_HEIGHT 360.0f
// #define GAME_CAMERA_WIDTH 640.0f
#define GAME_CAMERA_HEIGHT 180.0f
#define GAME_CAMERA_WIDTH 320.0f
#define TICKS_PER_SECOND 60
#define ELAPSED_TIME_PER_TICK (1000.0f / TICKS_PER_SECOND)

#define BEETLE_SPEED 1.5f
#define BEETLE_JUMP_SPEED 2.0f

struct cg_state {
    // Game camera
    struct cc_camera camera;

    // Sprite shader
    struct cgl_sprite_shader sprite_shader;
    struct egl_sprite_buffer sprite_buffer;
    struct egl_texture sprite_atlas;

    // Tile shader
    struct cgl_tile_shader tile_shader;
    struct egl_tile_buffer tile_buffer;
    struct egl_texture tile_atlas;

    uint64_t delta_time_remaining;

    // Beetle
    ca_animation_id beetle_animation_walk;
    ca_animation_id beetle_animation_idle;

    struct ce_entity beetle_a;
    struct ce_entity beetle_b;

    struct cs_spritemap spritemap;
    struct ct_tilemap tilemap;
    struct cb_bodymap bodymap;
    struct ca_animationmap animationmap;
};

ET_TEST(cg_state) {
    // Make sure it fits on persistent storage
    ET_ASSERT(sizeof(struct cg_state) < 10 * 1024 * 1024);
    ET_DONE;
}

int cg_init(void **out_data, struct em_arena *persistent_storage,
            struct em_arena *transient_storage) {
    int rc = 0;

    struct cg_state *state = (struct cg_state *)em_arena_alloc(
        persistent_storage, sizeof(struct cg_state));
    if (state == NULL) {
        el_critical("OOM");
        rc = -1;
        goto _err;
    }

    memset(state, 0, sizeof(struct cg_state));

    rc = cgl_sprite_shader_load(&state->sprite_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

    rc = cgl_tile_shader_load(&state->tile_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

    egl_sprite_buffer_create(CS_SPRITES_MAX, &state->sprite_buffer);
    egl_tile_buffer_create(CT_TILES_MAX, &state->tile_buffer);

    rc = egl_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    rc = egl_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initial state
    state->beetle_a.animation =
        ca_play(&state->animationmap, GEN_ANIMATION_BEETLE_WALK);
    state->beetle_a.animation_walk = GEN_ANIMATION_BEETLE_WALK;
    state->beetle_a.animation_idle = GEN_ANIMATION_BEETLE_IDLE;
    state->beetle_a.position.x = 100.0f;
    state->beetle_a.position.y = 100.0f;
    state->beetle_a.body = cb_add(
        &state->bodymap,
        &(struct cb_body){
            .position = {.x = state->beetle_a.position.x +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].x,
                         .y = state->beetle_a.position.y +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].y},
            .size = {.w = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].w,
                     .h = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].h},
        });
    state->beetle_a.sprite = cs_add(
        &state->spritemap,
        &(struct egl_sprite){
            .position = {100, 100},
            .size = {.w = gen_frame_atlas[GEN_FRAME_BEETLE_0].w,
                     .h = gen_frame_atlas[GEN_FRAME_BEETLE_0].h},
            .texture_offset = {.u = gen_frame_atlas[GEN_FRAME_BEETLE_0].u,
                               .v = gen_frame_atlas[GEN_FRAME_BEETLE_0].v},
        });

    state->beetle_b.animation =
        ca_play(&state->animationmap, state->beetle_animation_walk);
    state->beetle_b.animation_walk = state->beetle_animation_walk;
    state->beetle_b.animation_idle = state->beetle_animation_idle;
    state->beetle_b.position.x = 200.0f;
    state->beetle_b.position.y = 200.0f;
    state->beetle_b.body = cb_add(
        &state->bodymap,
        &(struct cb_body){
            .position = {.x = state->beetle_b.position.x +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].x,
                         .y = state->beetle_b.position.y +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].y},
            .size = {.w = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].w,
                     .h = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].h},
        });
    state->beetle_b.sprite = cs_add(
        &state->spritemap,
        &(struct egl_sprite){
            .position = {200, 200},
            .size = {.w = gen_frame_atlas[GEN_FRAME_BEETLE_0].w,
                     .h = gen_frame_atlas[GEN_FRAME_BEETLE_0].h},
            .texture_offset = {.u = gen_frame_atlas[GEN_FRAME_BEETLE_0].u,
                               .v = gen_frame_atlas[GEN_FRAME_BEETLE_0].v},
        });

    state->camera = (struct cc_camera){.x = state->beetle_a.position.x,
                                       .y = state->beetle_a.position.y,
                                       .w = GAME_CAMERA_WIDTH,
                                       .h = GAME_CAMERA_HEIGHT};

    // Set some tiles
    for (int i = 0; i < 60; i++) {
        ct_set(&state->tilemap, i, 40, CT_TILE_TYPE_GRASS);
    }

    // Load the VBOs
    egl_sprite_buffer_data(&state->sprite_buffer, state->spritemap.sprite_count,
                           state->spritemap.sprites);
    egl_tile_buffer_data(&state->tile_buffer, state->tilemap.tile_count,
                         state->tilemap.tiles);

    *out_data = (void *)state;
    goto _done;

_err:
    cg_destroy(state);

_done:
    return rc;
}

void cg_reload(void *data, struct em_arena *transient_storage) {
    struct cg_state *state = (struct cg_state *)data;
    cgl_sprite_shader_load(&state->sprite_shader, transient_storage);
    cgl_tile_shader_load(&state->tile_shader, transient_storage);
    egl_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    egl_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
}

void cg_tick(struct cg_state *state, struct egl_frame *frame) {
    ce_tick(&state->beetle_a, &state->animationmap, &state->bodymap,
            &state->tilemap);
    ce_tick(&state->beetle_b, &state->animationmap, &state->bodymap,
            &state->tilemap);
}

bool cg_frame(void *data, struct egl_frame *frame) {
    struct cg_state *state = (struct cg_state *)data;

    // Handle input
    SDL_Event sdl_event = {0};
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
            return false;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (sdl_event.key.repeat) {
                continue;
            }
            if (sdl_event.key.down) {
                switch (sdl_event.key.key) {
                case SDLK_Q:
                    return false;

                case SDLK_W:
                    state->beetle_a.velocity.y = -BEETLE_JUMP_SPEED;
                    break;
                case SDLK_A:
                    state->beetle_a.velocity.x = -BEETLE_SPEED;
                    break;
                case SDLK_D:
                    state->beetle_a.velocity.x = BEETLE_SPEED;
                    break;
                }
                break;
            } else {
                switch (sdl_event.key.key) {
                case SDLK_A:
                case SDLK_D:
                    state->beetle_a.velocity.x = 0;
                    break;
                }
                break;
            }
        }
    }

    bool tilemap_dirty = false;

    // Place tiles
    struct eu_fpos win_pos;
    SDL_MouseButtonFlags mouse_flags =
        SDL_GetMouseState(&win_pos.x, &win_pos.y);
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0) {
        struct cc_bounds cam_bounds;
        cc_bounds(&cam_bounds, &state->camera);
        struct eu_isize win_size;
        SDL_GetWindowSize(frame->sdl_window, &win_size.w, &win_size.h);
        struct eu_ixpos tile_pos;
        struct eu_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                     GEN_TILE_ATLAS_TILE_SIZE};
        ct_screen_pos(&tile_pos, &cam_bounds, &win_size, &win_pos, &tile_size);
        ct_set(&state->tilemap, tile_pos.x, tile_pos.y, CT_TILE_TYPE_GRASS);
        tilemap_dirty = true;
    }

    // Remove tiles
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
        struct cc_bounds cam_bounds;
        cc_bounds(&cam_bounds, &state->camera);
        struct eu_isize win_size;
        SDL_GetWindowSize(frame->sdl_window, &win_size.w, &win_size.h);
        struct eu_ixpos tile_pos;
        struct eu_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                     GEN_TILE_ATLAS_TILE_SIZE};
        ct_screen_pos(&tile_pos, &cam_bounds, &win_size, &win_pos, &tile_size);
        ct_set(&state->tilemap, tile_pos.x, tile_pos.y, CT_TILE_TYPE_EMPTY);
        tilemap_dirty = true;
    }

    // Fixed time ticks
    state->delta_time_remaining += frame->delta_time;
    while (state->delta_time_remaining > ELAPSED_TIME_PER_TICK) {
        state->delta_time_remaining -= ELAPSED_TIME_PER_TICK;
        struct egl_frame tick_frame = {
            .delta_time = ELAPSED_TIME_PER_TICK,
            .sdl_window = frame->sdl_window,
        };
        cg_tick(state, &tick_frame);
    }

    // Move sprite
    ce_frame(&state->beetle_a, &state->animationmap, &state->spritemap,
             frame->delta_time);
    ce_frame(&state->beetle_b, &state->animationmap, &state->spritemap,
             frame->delta_time);

    // Update the VBOs
    egl_sprite_buffer_data(&state->sprite_buffer, state->spritemap.sprite_count,
                           state->spritemap.sprites);
    if (tilemap_dirty) {
        egl_tile_buffer_data(&state->tile_buffer, state->tilemap.tile_count,
                             state->tilemap.tiles);
    }

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Follow player
    state->camera.x = state->beetle_a.position.x;
    state->camera.y = state->beetle_a.position.y;

    // Orthographic projection camera
    struct eu_mat4 camera_transform = {0.0f};
    eu_mat4_ortho_camera(&camera_transform, GAME_CAMERA_WIDTH,
                         GAME_CAMERA_HEIGHT, state->camera.x, state->camera.y);

    // Render tiles
    struct eu_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
                                 GEN_TILE_ATLAS_TILE_SIZE};
    cgl_tile_shader_render(&state->tile_shader, &tile_size, &camera_transform,
                           &state->tile_atlas, state->tilemap.tile_count,
                           &state->tile_buffer);

    // Render sprites
    cgl_sprite_shader_render(
        &state->sprite_shader, &camera_transform, &state->sprite_atlas,
        state->spritemap.sprite_count, &state->sprite_buffer);

    return true;
}

void cg_destroy(void *data) {
    struct cg_state *state = (struct cg_state *)data;
    cgl_sprite_shader_destroy(&state->sprite_shader);
    cgl_tile_shader_destroy(&state->tile_shader);
    egl_sprite_buffer_destroy(&state->sprite_buffer);
    egl_tile_buffer_destroy(&state->tile_buffer);
    egl_texture_destroy(&state->sprite_atlas);
    egl_texture_destroy(&state->tile_atlas);
}
