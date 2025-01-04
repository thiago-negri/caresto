#include <SDL3/SDL.h>

#include <engine/egl_opengl.h>
#include <engine/el_log.h>

#include <caresto/cg_game.h>
#include <caresto/cs_shaders.h>

#define SPRITE_ATLAS_PATH "assets/sprite_atlas.png"
#define GAME_CAMERA_HEIGHT 360.0f
#define GAME_CAMERA_WIDTH 640.0f
#define TILE_SIZE 8

#define CG_TILES_MAX_WIDTH 100
#define CG_TILES_MAX_HEIGHT 100
#define CG_SPRITES_MAX 1024

enum cg_tile {
    CG_TILE_EMPTY = 0,
    CG_TILE_FLOOR = 1,
};

struct cg_state {
    // Game camera
    struct egl_vec2 camera_position;

    // Sprite shader
    struct cs_sprite_shader sprite_shader;
    struct egl_sprite_buffer sprite_buffer;
    struct egl_texture sprite_atlas;

    // Sprites
    size_t sprite_count;
    struct egl_sprite sprites[CG_SPRITES_MAX];

    // World tilemap
    size_t tilemap_width;
    size_t tilemap_height;
    enum cg_tile tiles[CG_TILES_MAX_HEIGHT][CG_TILES_MAX_WIDTH];
};

int cg_init(void **out_data, struct em_arena *persistent_storage,
            struct em_arena *transient_storage) {
    int rc = 0;
    struct cs_sprite_shader sprite_shader = {0};
    struct egl_sprite_buffer sprite_buffer = {0};
    struct egl_texture sprite_atlas = {0};

    rc = cs_sprite_shader_load(&sprite_shader, transient_storage);
    if (rc != 0) {
        goto _err;
    }

    egl_sprite_buffer_create(CG_SPRITES_MAX, &sprite_buffer);

    struct egl_vec2 camera_position = {
        .x = (GLfloat)GAME_CAMERA_WIDTH * 0.5f,
        .y = (GLfloat)GAME_CAMERA_HEIGHT * 0.5f,
    };

    // TODO(tnegri): Bake atlas in
    rc = egl_texture_load(SPRITE_ATLAS_PATH, &sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initial state
    struct cg_state *state = (struct cg_state *)em_arena_alloc(
        persistent_storage, sizeof(struct cg_state));
    memset(state, 0, sizeof(struct cg_state));
    state->sprite_shader = sprite_shader;
    state->sprite_buffer = sprite_buffer;
    state->sprite_atlas = sprite_atlas;
    state->camera_position = camera_position;
    state->sprite_count = 1;
    state->sprites[0] = (struct egl_sprite){
        .x = 0.0f,
        .y = 0.0f,
        .w = 16,
        .h = 16,
        .u = 0,
        .v = 0,
    };

    *out_data = (void *)state;
    goto _done;

_err:
    cs_sprite_shader_destroy(&sprite_shader);
    egl_sprite_buffer_destroy(&sprite_buffer);
    egl_texture_destroy(&sprite_atlas);

_done:
    return rc;
}

void cg_reload(void *data, struct em_arena *transient_storage) {
    struct cg_state *state = (struct cg_state *)data;
    cs_sprite_shader_load(&state->sprite_shader, transient_storage);
    egl_texture_load(SPRITE_ATLAS_PATH, &state->sprite_atlas);
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
            if (sdl_event.key.key == SDLK_Q) {
                return false;
            }
            break;
        }
    }

    // Move sprite
    state->sprites[0].x = 10;
    state->sprites[0].y = 10;
    state->sprites[1] = (struct egl_sprite){
        .x = 100.0f,
        .y = 100.0f,
        .w = 489,
        .h = 509,
        .u = 100,
        .v = 100,
    };
    state->sprite_count = 1;

    // Update the VBO
    egl_sprite_buffer_data(&state->sprite_buffer, state->sprite_count,
                           state->sprites);

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Orthographic projection camera
    struct egl_mat4 camera_transform = {.values = {0.0f}};
    GLfloat half_width = GAME_CAMERA_WIDTH * 0.5f;
    GLfloat half_height = GAME_CAMERA_HEIGHT * 0.5f;
    GLfloat camera_x = half_width;
    GLfloat camera_y = half_height;
    GLfloat screen_top = camera_y - half_height;
    GLfloat screen_left = camera_x - half_width;
    GLfloat screen_right = camera_x + half_width;
    GLfloat screen_bottom = camera_y + half_height;
    egl_ortho(&camera_transform, screen_left, screen_right, screen_top,
              screen_bottom, 0.0f, 1.0f);

    // Render sprites
    cs_sprite_shader_render(&state->sprite_shader, &camera_transform,
                            &state->sprite_atlas, state->sprite_count,
                            &state->sprite_buffer);

    return true;
}

void cg_destroy(void *data) {
    struct cg_state *state = (struct cg_state *)data;
    cs_sprite_shader_destroy(&state->sprite_shader);
    egl_sprite_buffer_destroy(&state->sprite_buffer);
    egl_texture_destroy(&state->sprite_atlas);
}
