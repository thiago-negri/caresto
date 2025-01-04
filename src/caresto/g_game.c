#include <SDL3/SDL.h>

#include <caresto/g_game.h>
#include <caresto/gl_opengl.h>

// FIXME(tnegri): SPRITE_MAX is shared between main and g_game
#define SPRITE_MAX 1024

struct g_state {
    size_t sprite_count;
    struct gl_sprite sprites[SPRITE_MAX];
};

void *g_init(struct mm_arena *persistent_storage) {
    struct g_state *state = (struct g_state *)mm_arena_alloc(
        persistent_storage, sizeof(struct g_state));

    // Initial state
    state->sprite_count = 1;
    state->sprites[0] = (struct gl_sprite){
        .x = 0.0f,
        .y = 0.0f,
        .w = 16,
        .h = 16,
        .u = 0,
        .v = 0,
    };

    return (void *)state;
}

bool g_process_frame(struct g_frame *frame, void *data) {
    struct g_state *state = (struct g_state *)data;

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
    state->sprites[0].x += frame->delta_time / 100.0f;
    if (state->sprites[0].x > 100.0f) {
        state->sprites[0].x = 0.0f;
    }

    // Update the VBO
    gl_sprite_buffer_data(frame->sprite_buffer, state->sprite_count,
                          state->sprites);

    // Clear screen
    glClearColor(0.3f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render sprites
    gl_program_render(frame->program, frame->camera_transform,
                      frame->sprite_atlas, state->sprite_count,
                      frame->sprite_buffer);

    return true;
}
