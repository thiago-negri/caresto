#include <SDL3/SDL.h>
#include <caresto/data/cds_state.h>
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

#define cge_init ee_init
#define cge_reload ee_reload
#define cge_frame ee_frame
#define cge_destroy ee_destroy

int cge_init(void **out_data, struct ea_arena *persistent_storage,
             struct ea_arena *transient_storage) {
    int rc = 0;

    struct cds_state *state = (struct cds_state *)ea_arena_alloc(
        persistent_storage, sizeof(struct cds_state));
    if (state == NULL) {
        el_critical("OOM");
        rc = -1;
        goto _err;
    }

    memset(state, 0, sizeof(struct cds_state));

    rc = coo_sprite_shader_load(&state->sprite_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

    coo_sprite_buffer_create(&state->sprite_buffer, CSS_SPRITES_MAX);

    // TODO(tnegri): Make this a static buffer, recreate it when tiles change
    coo_sprite_buffer_create(&state->tile_buffer, 100);

    rc = eo_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initial state
    state->carestosan.type = cee_carestosan;
    css_set(&state->spritemap, &state->carestosan.sprite,
            &(struct css_sprite){
                .x = 100,
                .y = 100,
                .w = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].w,
                .h = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].h,
                .u = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].u,
                .v = gen_frame_atlas[GEN_FRAME_CARESTOSAN_4].v,
                .flags = 0,
            });
    csa_play(&state->carestosan.animation, &state->animationmap,
             GEN_ANIMATION_CARESTOSAN_IDLE, state->carestosan.sprite,
             &state->spritemap);
    state->carestosan.position.x = 100.0f;
    state->carestosan.position.y = 100.0f;
    state->carestosan.body = csb_add(
        &state->bodymap,
        &(struct csb_body){
            .position = {.x = state->carestosan.position.x +
                              gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].x,
                         .y = state->carestosan.position.y +
                              gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].y},
            .size = {.w = gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].w,
                     .h = gen_bounding_box_atlas[GEN_SPRITE_CARESTOSAN].h},
        });

    state->beetle.type = cee_beetle;
    css_set(&state->spritemap, &state->beetle.sprite,
            &(struct css_sprite){
                .x = 200,
                .y = 200,
                .w = gen_frame_atlas[GEN_FRAME_BEETLE_0].w,
                .h = gen_frame_atlas[GEN_FRAME_BEETLE_0].h,
                .u = gen_frame_atlas[GEN_FRAME_BEETLE_0].u,
                .v = gen_frame_atlas[GEN_FRAME_BEETLE_0].v,
                .flags = 0,
            });
    csa_play(&state->beetle.animation, &state->animationmap,
             GEN_ANIMATION_BEETLE_IDLE, state->beetle.sprite,
             &state->spritemap);
    state->beetle.position.x = 200.0f;
    state->beetle.position.y = 200.0f;
    state->beetle.body = csb_add(
        &state->bodymap,
        &(struct csb_body){
            .position = {.x = state->beetle.position.x +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].x,
                         .y = state->beetle.position.y +
                              gen_bounding_box_atlas[GEN_SPRITE_BEETLE].y},
            .size = {.w = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].w,
                     .h = gen_bounding_box_atlas[GEN_SPRITE_BEETLE].h},
        });

    state->camera = (struct csc_camera){.x = state->carestosan.position.x,
                                        .y = state->carestosan.position.y,
                                        .w = GAME_CAMERA_WIDTH,
                                        .h = GAME_CAMERA_HEIGHT};

    // Set some tiles
    for (int i = 0; i < 60; i++) {
        cst_set(&state->tilemap, i, 40, CST_SOLID);
    }

    // Load the VBOs
    eo_buffer_data(&state->sprite_buffer,
                   state->spritemap.sprite_count * sizeof(struct coo_sprite),
                   state->spritemap.sprites_gpu);
    // TODO(tnegri): Load tile buffer
    // eo_buffer_data(&state->tile_buffer, state->tilemap.tile_count,
    //                state->tilemap.tiles);

    *out_data = (void *)state;
    goto _done;

_err:
    cge_destroy(state);

_done:
    return rc;
}

void cge_reload(void *data, struct ea_arena *transient_storage) {
    struct cds_state *state = (struct cds_state *)data;
    coo_sprite_shader_load(&state->sprite_shader, transient_storage);
    eo_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    eo_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
}

void cge_tick(struct cds_state *state, struct eo_frame *frame) {
    csb_tick(&state->bodymap, &state->tilemap);
    cee_tick((union cee_entity *)&state->carestosan, &state->animationmap,
             &state->bodymap, &state->spritemap);
    cee_tick((union cee_entity *)&state->beetle, &state->animationmap,
             &state->bodymap, &state->spritemap);
}

bool cge_frame(void *data, const struct eo_frame *frame) {
    struct cds_state *state = (struct cds_state *)data;

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

                case SDLK_W: {
                    if (csb_grounded(&state->bodymap, &state->tilemap,
                                     state->carestosan.body)) {
                        struct csb_body *body =
                            csb_get(&state->bodymap, state->carestosan.body);
                        body->velocity.y = -BEETLE_JUMP_SPEED;
                    }
                    break;
                }

                case SDLK_A: {
                    struct csb_body *body =
                        csb_get(&state->bodymap, state->carestosan.body);
                    body->velocity.x = -BEETLE_SPEED;
                    break;
                }

                case SDLK_D: {
                    struct csb_body *body =
                        csb_get(&state->bodymap, state->carestosan.body);
                    body->velocity.x = BEETLE_SPEED;
                    break;
                }
                }
                break;
            } else {
                switch (sdl_event.key.key) {
                case SDLK_A:
                case SDLK_D:
                    struct csb_body *body =
                        csb_get(&state->bodymap, state->carestosan.body);
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
        csc_bounds(&cam_bounds, &state->camera);
        struct em_isize win_size;
        SDL_GetWindowSize(frame->sdl_window, &win_size.w, &win_size.h);
        struct em_ipos tile_pos;
        cst_screen_pos(&tile_pos, &cam_bounds, &win_size, &win_pos);
        cst_set(&state->tilemap, tile_pos.x, tile_pos.y, CST_SOLID);
        tilemap_dirty = true;
    }

    // Remove tiles
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
        struct csc_bounds cam_bounds;
        csc_bounds(&cam_bounds, &state->camera);
        struct em_isize win_size;
        SDL_GetWindowSize(frame->sdl_window, &win_size.w, &win_size.h);
        struct em_ipos tile_pos;
        cst_screen_pos(&tile_pos, &cam_bounds, &win_size, &win_pos);
        cst_set(&state->tilemap, tile_pos.x, tile_pos.y, CST_EMPTY);
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
        cge_tick(state, &tick_frame);
    }

    // Animate sprites
    csa_frame(&state->animationmap, frame->delta_time, &state->spritemap);

    // Update the VBOs
    eo_buffer_data(&state->sprite_buffer,
                   state->spritemap.sprite_count * sizeof(struct coo_sprite),
                   state->spritemap.sprites_gpu);
    // TODO(tnegri): Load tilemap
    // if (tilemap_dirty) {
    //     eo_tile_buffer_data(&state->tile_buffer, state->tilemap.tile_count,
    //                         state->tilemap.tiles);
    // }

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Follow player
    state->camera.x = state->carestosan.position.x;
    state->camera.y = state->carestosan.position.y;

    // Orthographic projection camera
    struct em_mat4 camera_transform = {};
    em_mat4_ortho_camera(&camera_transform, GAME_CAMERA_WIDTH,
                         GAME_CAMERA_HEIGHT, state->camera.x, state->camera.y);

    // TODO(tnegri): Render tiles
    // struct em_isize tile_size = {GEN_TILE_ATLAS_TILE_SIZE,
    //                              GEN_TILE_ATLAS_TILE_SIZE};
    // coo_tile_shader_render(&state->tile_shader, &tile_size,
    // &camera_transform,
    //                        &state->tile_atlas, state->tilemap.tile_count,
    //                        &state->tile_buffer);

    // Render sprites
    coo_sprite_shader_render(
        &state->sprite_shader, &camera_transform, &state->sprite_atlas,
        state->spritemap.sprite_count * 6, &state->sprite_buffer);

    return true;
}

void cge_destroy(void *data) {
    struct cds_state *state = (struct cds_state *)data;
    coo_sprite_shader_destroy(&state->sprite_shader);
    eo_buffer_destroy(&state->sprite_buffer);
    eo_buffer_destroy(&state->tile_buffer);
    eo_texture_destroy(&state->sprite_atlas);
    eo_texture_destroy(&state->tile_atlas);
}
