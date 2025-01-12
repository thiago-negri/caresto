#pragma once

#include <engine/ea_allocator.h>
#include <engine/eo_opengl.h>
#include <stdint.h>

#if defined(SHARED) && defined(CARESTO_MAIN)

int (*cgl_init)(void **, struct ea_arena *, struct ea_arena *);
void (*cgl_reload)(void *, struct ea_arena *);
bool (*cgl_frame)(void *, struct eo_frame *);
void (*cgl_destroy)(void *);

#else // not (defined(SHARED) && defined(CARESTO_MAIN))

#if defined(SHARED) && defined(_WIN32)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

DLLEXPORT int cgl_init(void **out_data, struct ea_arena *persistent_storage,
                       struct ea_arena *transient_storage);
DLLEXPORT void cgl_reload(void *data, struct ea_arena *transient_storage);
DLLEXPORT bool cgl_frame(void *data, struct eo_frame *frame);
DLLEXPORT void cgl_destroy(void *data);

#endif // not (defined(SHARED) && defined(CARESTO_MAIN))
