#ifndef CG_GAME_H
#define CG_GAME_H

#include <stdbool.h>
#include <stdint.h>

#include <engine/egl_opengl.h>
#include <engine/em_memory.h>

#if defined(SHARED) && defined(CARESTO_MAIN)

int (*cg_init_ptr)(void **, struct em_arena *, struct em_arena *);
void (*cg_reload_ptr)(void *, struct em_arena *);
bool (*cg_frame_ptr)(void *, struct egl_frame *);
void (*cg_destroy_ptr)(void *);

int cg_init(void **out_data, struct em_arena *persistent_storage,
            struct em_arena *transient_storage) {
    return cg_init_ptr(out_data, persistent_storage, transient_storage);
}

void cg_reload(void *data, struct em_arena *transient_storage) {
    return cg_reload_ptr(data, transient_storage);
}

bool cg_frame(void *data, struct egl_frame *frame) {
    return cg_frame_ptr(data, frame);
}

void cg_destroy(void *data) { cg_destroy_ptr(data); }

#else

#if defined(SHARED) && defined(_WIN32)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // defined(SHARED) && defined(_WIN32)

DLLEXPORT int cg_init(void **out_data, struct em_arena *persistent_storage,
                      struct em_arena *transient_storage);
DLLEXPORT void cg_reload(void *data, struct em_arena *transient_storage);
DLLEXPORT bool cg_frame(void *data, struct egl_frame *frame);
DLLEXPORT void cg_destroy(void *data);

#endif // defined(SHARED) && defined(CARESTO_MAIN)

#endif // CG_GAME_H
