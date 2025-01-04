#include <SDL3/SDL.h>

#include <engine/egl_opengl.h>
#include <engine/el_log.h>

#include <caresto/cg_game.h>
#include <caresto/cs_shaders.h>

#define SPRITE_MAX 1024

struct cg_state {
    struct cs_sprite_shader sprite_shader;
    struct egl_sprite_buffer sprite_buffer;
    struct egl_texture sprite_atlas;
    struct egl_mat4 camera_transform;
    size_t sprite_count;
    struct egl_sprite sprites[SPRITE_MAX];
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

    egl_sprite_buffer_create(SPRITE_MAX, &sprite_buffer);

    // Set orthographic projection camera
    // 640x360 is the perfect res for pixel art games because it scales evenly
    // to all target resolutions.  We need to start the window at user's native
    // res though.  We use a 640x360 to paint the game, then scale it to native
    // res.  GUI should be painted on the native res surface.
    GLfloat screen_width = 640.0f;
    GLfloat screen_height = 360.0f;
    struct egl_mat4 camera_transform = {.values = {0.0f}};
    egl_ortho(&camera_transform, 0.0f, screen_width, 0.0f, screen_height, 0.0f,
              1.0f);

    // TODO(tnegri): Bake atlas in
    rc = egl_texture_load("assets/sprite_atlas.png", &sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initial state
    struct cg_state *state = (struct cg_state *)em_arena_alloc(
        persistent_storage, sizeof(struct cg_state));
    state->sprite_shader = sprite_shader;
    state->sprite_buffer = sprite_buffer;
    state->sprite_atlas = sprite_atlas;
    state->camera_transform = camera_transform;
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
    state->sprites[0].x += frame->delta_time / 10.0f;
    state->sprites[0].y -= frame->delta_time / 10.0f;
    if (state->sprites[0].x > 100.0f) {
        state->sprites[0].x = 0.0f;
    }
    if (state->sprites[0].y < -100.0f) {
        state->sprites[0].y = 0.0f;
    }

    // Update the VBO
    egl_sprite_buffer_data(&state->sprite_buffer, state->sprite_count,
                           state->sprites);

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render sprites
    cs_sprite_shader_render(&state->sprite_shader, &state->camera_transform,
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
