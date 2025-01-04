#ifndef G_GAME_H
#define G_GAME_H

#include <stdint.h>
#include <stdbool.h>

#include <engine/gl_opengl.h>
#include <engine/mm_memory_management.h>

// That should probably go to engine?
struct g_frame {
    uint64_t delta_time;

    // These should probably be somewhere else?
    struct gl_program *program;
    struct gl_mat4 *camera_transform;
    struct gl_texture *sprite_atlas;
    struct gl_sprite_buffer *sprite_buffer;
};

#ifdef DEBUG

#   ifdef CARESTO_MAIN

void *(*g_init_ptr)(struct mm_arena *persistent_storage);
bool (*g_process_frame_ptr)(struct g_frame *frame, void *data);

void *g_init(struct mm_arena *persistent_storage) {
    return g_init_ptr(persistent_storage);
}

bool g_process_frame(struct g_frame *frame, void *data) {
    return g_process_frame_ptr(frame, data);
}

#   else

#       ifdef _WIN32
#           define DLLEXPORT __declspec(dllexport)
#       else
#           define DLLEXPORT
#       endif // _WIN32

DLLEXPORT void *g_init(struct mm_arena *persistent_storage);
DLLEXPORT bool g_process_frame(struct g_frame *frame, void *data);

#   endif // CARESTO_MAIN

#else

void *g_init(struct mm_arena *persistent_storage);
bool g_process_frame(struct g_frame *frame, void *data);

#endif // DEBUG

#endif // G_GAME_H
