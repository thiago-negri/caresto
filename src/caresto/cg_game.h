#ifndef CG_GAME_H
#define CG_GAME_H

#include <stdbool.h>
#include <stdint.h>

#include <engine/egl_opengl.h>
#include <engine/em_memory.h>

#if defined(SHARED) && defined(CARESTO_MAIN)

int (*cg_init)(void **, struct em_arena *, struct em_arena *);
void (*cg_reload)(void *, struct em_arena *);
bool (*cg_frame)(void *, struct egl_frame *);
void (*cg_destroy)(void *);

#else // not (defined(SHARED) && defined(CARESTO_MAIN))

#if defined(SHARED) && defined(_WIN32)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

DLLEXPORT int cg_init(void **out_data, struct em_arena *persistent_storage,
                      struct em_arena *transient_storage);
DLLEXPORT void cg_reload(void *data, struct em_arena *transient_storage);
DLLEXPORT bool cg_frame(void *data, struct egl_frame *frame);
DLLEXPORT void cg_destroy(void *data);

#endif // not (defined(SHARED) && defined(CARESTO_MAIN))

#endif // CG_GAME_H
