#include <SDL3/SDL.h>

#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/et_test.h>

#include <caresto/cg_game.h>
#include <caresto/cgl_opengl.h>
#include <caresto/cs_sprite.h>
#include <caresto/ct_tilemap.h>

#include <gen/sprite_atlas.h>
#include <gen/tile_atlas.h>

#define GAME_CAMERA_HEIGHT 360.0f
#define GAME_CAMERA_WIDTH 640.0f
#define TICKS_PER_SECOND 60
#define ELAPSED_TIME_PER_TICK (1000.0f / TICKS_PER_SECOND)

#define BEETLE_SPEED 0.5f
struct beetle {
    sprite_id sprite;
    struct egl_ivec2 position;
    struct egl_vec2 position_remaining;
    struct egl_vec2 velocity;
};

struct cg_state {
    // Game camera
    struct egl_vec2 camera_position;

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
    struct beetle beetle;

    struct cs_sprite sprite;

    struct ct_tilemap tilemap;
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

    state->camera_position = (struct egl_vec2){
        .x = (GLfloat)GAME_CAMERA_WIDTH * 0.5f,
        .y = (GLfloat)GAME_CAMERA_HEIGHT * 0.5f,
    };

    // TODO(tnegri): Bake atlas in
    rc = egl_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    rc = egl_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initial state
    state->beetle.sprite =
        cs_add(&state->sprite, &(struct egl_sprite){
                                   .x = 100.0f,
                                   .y = 100.0f,
                                   .w = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_W,
                                   .h = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_H,
                                   .u = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_U,
                                   .v = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_V,
                               });

    // Set some tiles
    ct_set(&state->tilemap, 0, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 1, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 2, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 3, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 4, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 5, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 6, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 7, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 8, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 9, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 10, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 11, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 12, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 13, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 14, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 15, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 16, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 17, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 18, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 19, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 20, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 21, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 22, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 23, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 24, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 25, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 26, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 27, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 28, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 29, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 30, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 31, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 32, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 33, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 34, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 35, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 36, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 37, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 38, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 39, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 40, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 41, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 42, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 43, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 44, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 45, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 46, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 47, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 48, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 49, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 50, 40, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 51, 40, CT_TILE_TYPE_GRASS);

    // Load the VBOs
    egl_sprite_buffer_data(&state->sprite_buffer, state->sprite.sprite_count,
                           state->sprite.sprites);
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
    state->beetle.position_remaining.x += state->beetle.velocity.x;
    state->beetle.position_remaining.y += state->beetle.velocity.y;

    state->beetle.position.x += state->beetle.position_remaining.x;
    state->beetle.position.y += state->beetle.position_remaining.y;

    state->beetle.position_remaining.x -=
        (int)state->beetle.position_remaining.x;
    state->beetle.position_remaining.y -=
        (int)state->beetle.position_remaining.y;
}

bool cg_frame(void *data, struct egl_frame *frame) {
    struct cg_state *state = (struct cg_state *)data;

    // Orthographic projection camera
    GLfloat half_width = (GLfloat)GAME_CAMERA_WIDTH * 0.5f;
    GLfloat half_height = (GLfloat)GAME_CAMERA_HEIGHT * 0.5f;
    GLfloat camera_x = state->camera_position.x;
    GLfloat camera_y = state->camera_position.y;
    GLfloat screen_top = camera_y - half_height;
    GLfloat screen_left = camera_x - half_width;
    GLfloat screen_right = camera_x + half_width;
    GLfloat screen_bottom = camera_y + half_height;
    struct egl_mat4 camera_transform = {.values = {0.0f}};
    egl_ortho(&camera_transform, screen_left, screen_right, screen_top,
              screen_bottom, 0.0f, 1.0f);

    bool tilemap_dirty = false;

    // Handle input
    SDL_Event sdl_event = {0};
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
            return false;

        case SDL_EVENT_KEY_DOWN:
            switch (sdl_event.key.key) {
            case SDLK_Q:
                return false;

            case SDLK_W:
                state->beetle.velocity.y = -BEETLE_SPEED;
                break;
            case SDLK_S:
                state->beetle.velocity.y = BEETLE_SPEED;
                break;
            case SDLK_A:
                state->beetle.velocity.x = -BEETLE_SPEED;
                break;
            case SDLK_D:
                state->beetle.velocity.x = BEETLE_SPEED;
                break;
            }
            break;

        case SDL_EVENT_KEY_UP:
            switch (sdl_event.key.key) {
            case SDLK_W:
            case SDLK_S:
                state->beetle.velocity.y = 0;
                break;
            case SDLK_A:
            case SDLK_D:
                state->beetle.velocity.x = 0;
                break;
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            // Place tiles
            float mouse_x, mouse_y;
            SDL_MouseButtonFlags mouse_flags =
                SDL_GetMouseState(&mouse_x, &mouse_y);
            if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0) {
                size_t tile_x, tile_y;
                int window_w, window_h;
                SDL_GetWindowSize(frame->sdl_window, &window_w, &window_h);
                ct_tile_pos(screen_left, screen_right, screen_top,
                            screen_bottom, window_w, window_h, mouse_x, mouse_y,
                            GEN_TILE_ATLAS_TILE_SIZE, GEN_TILE_ATLAS_TILE_SIZE,
                            &tile_x, &tile_y);
                ct_set(&state->tilemap, tile_x, tile_y, CT_TILE_TYPE_GRASS);
                tilemap_dirty = true;
            }

            // Remove tiles
            if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
                size_t tile_x, tile_y;
                int window_w, window_h;
                SDL_GetWindowSize(frame->sdl_window, &window_w, &window_h);
                ct_tile_pos(screen_left, screen_right, screen_top,
                            screen_bottom, window_w, window_h, mouse_x, mouse_y,
                            GEN_TILE_ATLAS_TILE_SIZE, GEN_TILE_ATLAS_TILE_SIZE,
                            &tile_x, &tile_y);
                ct_set(&state->tilemap, tile_x, tile_y, CT_TILE_TYPE_EMPTY);
                tilemap_dirty = true;
            }
            break;
        }
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
    struct egl_sprite *sprite = cs_get(&state->sprite, state->beetle.sprite);
    sprite->x = state->beetle.position.x;
    sprite->y = state->beetle.position.y;

    // Update the VBOs
    egl_sprite_buffer_data(&state->sprite_buffer, state->sprite.sprite_count,
                           state->sprite.sprites);
    if (tilemap_dirty) {
        egl_tile_buffer_data(&state->tile_buffer, state->tilemap.tile_count,
                             state->tilemap.tiles);
    }

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render tiles
    struct egl_ivec2 tile_size = {.values = {
                                      GEN_TILE_ATLAS_TILE_SIZE,
                                      GEN_TILE_ATLAS_TILE_SIZE,
                                  }};
    cgl_tile_shader_render(&state->tile_shader, &tile_size, &camera_transform,
                           &state->tile_atlas, state->tilemap.tile_count,
                           &state->tile_buffer);

    // Render sprites
    cgl_sprite_shader_render(&state->sprite_shader, &camera_transform,
                             &state->sprite_atlas, state->sprite.sprite_count,
                             &state->sprite_buffer);

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
