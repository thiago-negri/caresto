#ifndef CG_GAME_H
#define CG_GAME_H

#include <stdint.h>
#include <stdbool.h>

#include <engine/egl_opengl.h>
#include <engine/em_memory.h>

// FIXME(tnegri): How to avoid having to declare those twice?

#if defined(SHARED) && defined(CARESTO_MAIN)

void *(*g_init_ptr)(struct mm_arena *persistent_storage);
bool (*g_process_frame_ptr)(struct gl_frame *frame, void *data);

void *g_init(struct mm_arena *persistent_storage) {
    return g_init_ptr(persistent_storage);
}

bool g_process_frame(struct gl_frame *frame, void *data) {
    return g_process_frame_ptr(frame, data);
}

#else

#    if defined(SHARED) && defined(_WIN32)
#        define DLLEXPORT __declspec(dllexport)
#    else
#        define DLLEXPORT
#    endif // defined(SHARED) && defined(_WIN32)

DLLEXPORT void *g_init(struct mm_arena *persistent_storage);
DLLEXPORT bool g_process_frame(struct gl_frame *frame, void *data);

#endif // defined(SHARED) && defined(CARESTO_MAIN)

#endif // CG_GAME_H
