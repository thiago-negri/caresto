#include "caresto/system/csd_debug.h"
#include <SDL3/SDL.h>
#include <caresto/data/cdd_data.h>
#include <caresto/entity/cee_entity.h>
#include <caresto/opengl/coo_opengl.h>
#include <caresto/system/csa_animation.h>
#include <caresto/system/csb_body.h>
#include <caresto/system/csc_camera.h>
#include <caresto/system/css_sprite.h>
#include <caresto/system/cst_tile.h>
#include <engine/ee_entry.h>
#include <engine/ef_file.h>
#include <engine/el_log.h>
#include <engine/eo_opengl.h>
#include <engine/et_test.h>
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
#define DEBUG_SPRITES_MAX 100

#define cge_init ee_init
#define cge_reload ee_reload
#define cge_frame ee_frame
#define cge_destroy ee_destroy

int cge_init(void **out_data, struct ea_arena *persistent_storage,
             struct ea_arena *transient_storage) {
    int rc = 0;

    struct cdd_data *state = (struct cdd_data *)ea_arena_alloc(
        persistent_storage, sizeof(struct cdd_data));
    if (state == NULL) {
        el_critical("OOM");
        rc = -1;
        goto _err;
    }

    memset(state, 0, sizeof(struct cdd_data));

    rc = coo_sprite_shader_load(&state->sprite_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

#ifdef DEBUG
    rc = coo_debug_shader_load(&state->debug_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }
    coo_debug_buffer_create(&state->debug_buffer, CDS_DEBUG_MAX);
#endif // DEBUG

    coo_sprite_buffer_create(&state->sprite_buffer, CDS_SPRITES_MAX,
                             GL_STREAM_DRAW);

    coo_sprite_buffer_create(&state->tile_buffer, CDS_TILES_MAX,
                             GL_DYNAMIC_DRAW);

    rc = eo_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initial state
    state->systems.carestosan.type = cds_entity_carestosan;
    css_set(&state->systems, &state->systems.carestosan.sprite,
            &(struct cds_sprite){
                .x = 100,
                .y = 100,
                .w = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].w,
                .h = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].h,
                .u = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].u,
                .v = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].v,
                .flags = 0,
            });
    csa_play(&state->systems, &state->systems.carestosan.animation,
             GEN_ANIMATION_CARESTOSAN_IDLE, state->systems.carestosan.sprite);
    state->systems.carestosan.position.x = 100.0f;
    state->systems.carestosan.position.y = 100.0f;
    state->systems.carestosan.body = csb_add(
        &state->systems,
        &(struct cds_body){
            .position = {.x = state->systems.carestosan.position.x +
                              gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].x,
                         .y = state->systems.carestosan.position.y +
                              gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].y},
            .size = {.w = gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].w,
                     .h = gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].h},
        });

    state->systems.beetle.type = cds_entity_beetle;
    css_set(&state->systems, &state->systems.beetle.sprite,
            &(struct cds_sprite){
                .x = 200,
                .y = 200,
                .w = gen_frame_atlas[GEN_FRAME_BEETLE_0].w,
                .h = gen_frame_atlas[GEN_FRAME_BEETLE_0].h,
                .u = gen_frame_atlas[GEN_FRAME_BEETLE_0].u,
                .v = gen_frame_atlas[GEN_FRAME_BEETLE_0].v,
                .flags = 0,
            });
    csa_play(&state->systems, &state->systems.beetle.animation,
             GEN_ANIMATION_BEETLE_IDLE, state->systems.beetle.sprite);
    state->systems.beetle.position.x = 200.0f;
    state->systems.beetle.position.y = 200.0f;
    state->systems.beetle.body = csb_add(
        &state->systems,
        &(struct cds_body){
            .position = {.x = state->systems.beetle.position.x +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].x,
                         .y = state->systems.beetle.position.y +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].y},
            .size = {.w = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].w,
                     .h = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].h},
        });

    state->systems.camera =
        (struct cds_camera){.x = state->systems.carestosan.position.x,
                            .y = state->systems.carestosan.position.y,
                            .w = GAME_CAMERA_WIDTH,
                            .h = GAME_CAMERA_HEIGHT};

    // Set some tiles
    for (int i = 0; i < 60; i++) {
        cst_set(&state->systems.tile_map, i, 40, CDS_SOLID);
    }
    cst_redraw(&state->systems.tile_map);

    // Load the VBOs
    eo_buffer_data(&state->sprite_buffer,
                   state->systems.sprite_map.sprite_count *
                       sizeof(struct coo_sprite),
                   state->systems.sprite_map.sprites_gpu);
    eo_buffer_data(&state->tile_buffer,
                   state->systems.tile_map.tile_count *
                       sizeof(struct coo_sprite),
                   state->systems.tile_map.tiles_gpu);
#ifdef DEBUG
    eo_buffer_data(&state->debug_buffer,
                   state->systems.debug.debug_count * sizeof(struct coo_debug),
                   state->systems.debug.debug_gpu);
#endif // DEBUG

    *out_data = (void *)state;
    goto _done;

_err:
    cge_destroy(state);

_done:
    return rc;
}

void cge_reload(void *data, struct ea_arena *transient_storage) {
    struct cdd_data *state = (struct cdd_data *)data;
    coo_sprite_shader_load(&state->sprite_shader, transient_storage);
#ifdef DEBUG
    coo_debug_shader_load(&state->debug_shader, transient_storage);
#endif // DEBUG
    eo_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    eo_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
}

void cge_tick(struct cds_systems *systems, struct eo_frame *frame) {
    csb_tick(systems);
    // TODO(tnegri): odd
    cee_tick((union cds_entity *)&systems->carestosan, systems);
    cee_tick((union cds_entity *)&systems->beetle, systems);
}

bool cge_frame(void *data, const struct eo_frame *frame) {
    struct cdd_data *state = (struct cdd_data *)data;

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

#ifdef DEBUG
                case SDLK_B:
                    state->systems.debug_enabled =
                        !state->systems.debug_enabled;
                    break;
#endif // DEBUG

                case SDLK_W: {
                    if (csb_grounded(&state->systems,
                                     state->systems.carestosan.body)) {
                        struct cds_body *body = csb_get(
                            &state->systems, state->systems.carestosan.body);
                        body->velocity.y = -BEETLE_JUMP_SPEED;
                    }
                    break;
                }

                case SDLK_A: {
                    struct cds_body *body = csb_get(
                        &state->systems, state->systems.carestosan.body);
                    body->velocity.x = -BEETLE_SPEED;
                    break;
                }

                case SDLK_D: {
                    struct cds_body *body = csb_get(
                        &state->systems, state->systems.carestosan.body);
                    body->velocity.x = BEETLE_SPEED;
                    break;
                }
                }
                break;
            } else {
                switch (sdl_event.key.key) {
                case SDLK_A:
                case SDLK_D:
                    struct cds_body *body = csb_get(
                        &state->systems, state->systems.carestosan.body);
                    body->velocity.x = 0;
                    break;
                }
                break;
            }
        }
    }

    bool tilemap_dirty = false;

    // Place tiles
    struct em_fpos win_pos;
    SDL_MouseButtonFlags mouse_flags =
        SDL_GetMouseState(&win_pos.x, &win_pos.y);
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0) {
        struct csc_bounds cam_bounds;
        csc_bounds(&cam_bounds, &state->systems.camera);
        struct em_isize win_size;
        SDL_GetWindowSize(frame->sdl_window, &win_size.w, &win_size.h);
        struct em_ipos tile_pos;
        cst_screen_pos(&tile_pos, &cam_bounds, &win_size, &win_pos);
        cst_set(&state->systems.tile_map, tile_pos.x, tile_pos.y, CDS_SOLID);
        tilemap_dirty = true;
    }

    // Remove tiles
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
        struct csc_bounds cam_bounds;
        csc_bounds(&cam_bounds, &state->systems.camera);
        struct em_isize win_size;
        SDL_GetWindowSize(frame->sdl_window, &win_size.w, &win_size.h);
        struct em_ipos tile_pos;
        cst_screen_pos(&tile_pos, &cam_bounds, &win_size, &win_pos);
        cst_set(&state->systems.tile_map, tile_pos.x, tile_pos.y, CDS_EMPTY);
        tilemap_dirty = true;
    }

    // Fixed time ticks
    state->delta_time_remaining += frame->delta_time;
    while (state->delta_time_remaining > ELAPSED_TIME_PER_TICK) {
        state->delta_time_remaining -= ELAPSED_TIME_PER_TICK;
        struct eo_frame tick_frame = {
            .delta_time = ELAPSED_TIME_PER_TICK,
            .sdl_window = frame->sdl_window,
        };
        cge_tick(&state->systems, &tick_frame);
    }

    // Animate sprites
    csa_frame(&state->systems, frame->delta_time);

    // Update the VBOs
    eo_buffer_data(&state->sprite_buffer,
                   state->systems.sprite_map.sprite_count *
                       sizeof(struct coo_sprite),
                   state->systems.sprite_map.sprites_gpu);

    if (tilemap_dirty) {
        cst_redraw(&state->systems.tile_map);
        eo_buffer_data(&state->tile_buffer,
                       state->systems.tile_map.tile_count *
                           sizeof(struct coo_sprite),
                       state->systems.tile_map.tiles_gpu);
    }

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Follow player
    state->systems.camera.x = state->systems.carestosan.position.x;
    state->systems.camera.y = state->systems.carestosan.position.y;

    // Orthographic projection camera
    struct em_mat4 camera_transform = {};
    em_mat4_ortho_camera(&camera_transform, GAME_CAMERA_WIDTH,
                         GAME_CAMERA_HEIGHT, state->systems.camera.x,
                         state->systems.camera.y);

    // Render tiles
    coo_sprite_shader_render(
        &state->sprite_shader, &camera_transform, &state->tile_atlas,
        state->systems.tile_map.tile_count * COO_VERTEX_PER_QUAD,
        &state->tile_buffer);

    // Render sprites
    coo_sprite_shader_render(
        &state->sprite_shader, &camera_transform, &state->sprite_atlas,
        state->systems.sprite_map.sprite_count * COO_VERTEX_PER_QUAD,
        &state->sprite_buffer);

#ifdef DEBUG
    // Render debug
    if (state->systems.debug_enabled) {
        for (int i = 0; i < state->systems.body_map.body_count; i++) {
            struct cds_body *body = &state->systems.body_map.bodies[i];
            csd_quad(&state->systems, body->position.x, body->position.y,
                     body->size.w, body->size.h);
        }
        eo_buffer_data(&state->debug_buffer,
                       state->systems.debug.debug_count *
                           sizeof(struct coo_debug),
                       state->systems.debug.debug_gpu);
        coo_debug_shader_render(&state->debug_shader, &camera_transform,
                                state->systems.debug.debug_count *
                                    COO_VERTEX_PER_QUAD,
                                &state->debug_buffer);
        // reset debug draws
        state->systems.debug.debug_count = 0;
    }
#endif // DEBUG

    return true;
}

void cge_destroy(void *data) {
    struct cdd_data *state = (struct cdd_data *)data;
    coo_sprite_shader_destroy(&state->sprite_shader);
    eo_buffer_destroy(&state->sprite_buffer);
    eo_buffer_destroy(&state->tile_buffer);
    eo_texture_destroy(&state->sprite_atlas);
    eo_texture_destroy(&state->tile_atlas);
#ifdef DEBUG
    coo_debug_shader_destroy(&state->debug_shader);
    eo_buffer_destroy(&state->debug_buffer);
#endif
}
