#include <SDL3/SDL.h>

#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/et_test.h>

#include <caresto/cg_game.h>
#include <caresto/cs_shaders.h>
#include <caresto/ct_tilemap.h>

#include <gen/sprite_atlas.h>
#include <gen/tile_atlas.h>

#define GAME_CAMERA_HEIGHT 360.0f
#define GAME_CAMERA_WIDTH 640.0f

#define CG_SPRITES_MAX 1024

struct cg_state {
    // Game camera
    struct egl_vec2 camera_position;

    // Sprite shader
    struct cs_sprite_shader sprite_shader;
    struct egl_sprite_buffer sprite_buffer;
    struct egl_texture sprite_atlas;

    // Tile shader
    struct cs_tile_shader tile_shader;
    struct egl_tile_buffer tile_buffer;
    struct egl_texture tile_atlas;

    // Sprites
    size_t sprite_count;
    struct egl_sprite sprites[CG_SPRITES_MAX];

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

    rc = cs_sprite_shader_load(&state->sprite_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

    rc = cs_tile_shader_load(&state->tile_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

    egl_sprite_buffer_create(CG_SPRITES_MAX, &state->sprite_buffer);
    egl_tile_buffer_create(TILES_MAX, &state->tile_buffer);

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
    state->sprite_count = 1;
    state->sprites[0] = (struct egl_sprite){
        .x = 100.0f,
        .y = 100.0f,
        .w = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_W,
        .h = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_H,
        .u = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_U,
        .v = GEN_SPRITE_ATLAS_BEETLE_IDLE_0_V,
    };

    // Set some tiles
    ct_set(&state->tilemap, 0, 0, CT_TILE_TYPE_GRASS);
    ct_set(&state->tilemap, 0, 1, CT_TILE_TYPE_SAND);
    ct_set(&state->tilemap, 0, 2, CT_TILE_TYPE_WATER);
    ct_set(&state->tilemap, 0, 3, CT_TILE_TYPE_WATER);
    ct_set(&state->tilemap, 0, 4, CT_TILE_TYPE_WATER);

    // Load the VBOs
    egl_sprite_buffer_data(&state->sprite_buffer, state->sprite_count,
                           state->sprites);
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
    cs_sprite_shader_load(&state->sprite_shader, transient_storage);
    cs_tile_shader_load(&state->tile_shader, transient_storage);
    egl_texture_load(GEN_SPRITE_ATLAS_PATH, &state->sprite_atlas);
    egl_texture_load(GEN_TILE_ATLAS_PATH, &state->tile_atlas);
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

    // Handle input
    SDL_Event sdl_event = {0};
    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
            return false;

        case SDL_EVENT_KEY_DOWN:
            if (sdl_event.key.key == SDLK_Q) {
                return false;
            }
            break;
        }
    }

    // Place tiles
    bool tilemap_dirty = false;
    float mouse_x, mouse_y;
    SDL_MouseButtonFlags mouse_flags = SDL_GetMouseState(&mouse_x, &mouse_y);
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0) {
        size_t tile_x, tile_y;
        int window_w, window_h;
        SDL_GetWindowSize(frame->sdl_window, &window_w, &window_h);
        ct_tile_pos(screen_left, screen_right, screen_top, screen_bottom,
                    window_w, window_h, mouse_x, mouse_y,
                    GEN_TILE_ATLAS_TILE_SIZE, GEN_TILE_ATLAS_TILE_SIZE, &tile_x,
                    &tile_y);
        ct_set(&state->tilemap, tile_x, tile_y, CT_TILE_TYPE_GRASS);
        tilemap_dirty = true;
    }

    // Remove tiles
    if ((mouse_flags & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0) {
        size_t tile_x, tile_y;
        int window_w, window_h;
        SDL_GetWindowSize(frame->sdl_window, &window_w, &window_h);
        ct_tile_pos(screen_left, screen_right, screen_top, screen_bottom,
                    window_w, window_h, mouse_x, mouse_y,
                    GEN_TILE_ATLAS_TILE_SIZE, GEN_TILE_ATLAS_TILE_SIZE, &tile_x,
                    &tile_y);
        ct_set(&state->tilemap, tile_x, tile_y, CT_TILE_TYPE_EMPTY);
        tilemap_dirty = true;
    }

    // Move sprite
    state->sprites[0].x += frame->delta_time / 50.0f;
    state->sprites[0].y += frame->delta_time / 40.0f;
    if (state->sprites[0].x >= 200.0f) {
        state->sprites[0].x = 0.0f;
    }
    if (state->sprites[0].y >= 200.0f) {
        state->sprites[0].y = 0.0f;
    }
    state->sprite_count = 1;

    // Update the VBOs
    egl_sprite_buffer_data(&state->sprite_buffer, state->sprite_count,
                           state->sprites);
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
    cs_tile_shader_render(&state->tile_shader, &tile_size, &camera_transform,
                          &state->tile_atlas, state->tilemap.tile_count,
                          &state->tile_buffer);

    // Render sprites
    cs_sprite_shader_render(&state->sprite_shader, &camera_transform,
                            &state->sprite_atlas, state->sprite_count,
                            &state->sprite_buffer);

    return true;
}

void cg_destroy(void *data) {
    struct cg_state *state = (struct cg_state *)data;
    cs_sprite_shader_destroy(&state->sprite_shader);
    cs_tile_shader_destroy(&state->tile_shader);
    egl_sprite_buffer_destroy(&state->sprite_buffer);
    egl_tile_buffer_destroy(&state->tile_buffer);
    egl_texture_destroy(&state->sprite_atlas);
    egl_texture_destroy(&state->tile_atlas);
}
