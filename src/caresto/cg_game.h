#ifndef CG_GAME_H
#define CG_GAME_H

#include <stdbool.h>
#include <stdint.h>

#include <engine/egl_opengl.h>
#include <engine/em_memory.h>

// FIXME(tnegri): How to avoid having to declare those twice?

#if defined(SHARED) && defined(CARESTO_MAIN)

void *(*cg_init_ptr)(struct em_arena *persistent_storage);
bool (*cg_process_frame_ptr)(struct egl_frame *frame, void *data);

void *cg_init(struct em_arena *persistent_storage) {
    return cg_init_ptr(persistent_storage);
}

bool cg_process_frame(struct egl_frame *frame, void *data) {
    return cg_process_frame_ptr(frame, data);
}

#else

#if defined(SHARED) && defined(_WIN32)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // defined(SHARED) && defined(_WIN32)

DLLEXPORT void *cg_init(struct em_arena *persistent_storage);
DLLEXPORT bool cg_process_frame(struct egl_frame *frame, void *data);

#endif // defined(SHARED) && defined(CARESTO_MAIN)

#endif // CG_GAME_H
