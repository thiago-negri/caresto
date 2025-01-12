#pragma once

#include <engine/ea_allocator.h>
#include <engine/eo_opengl.h>

typedef int(ee_init_fn)(void **data, struct ea_arena *persistent,
                        struct ea_arena *transient);
typedef void(ee_reload_fn)(void *data, struct ea_arena *transient);
typedef bool(ee_frame_fn)(void *data, const struct eo_frame *frame);
typedef void(ee_destroy_fn)(void *data);

#if defined(SHARED) && defined(ENGINE_MAIN)

ee_init_fn *ee_init;
ee_reload_fn *ee_reload;
ee_frame_fn *ee_frame;
ee_destroy_fn *ee_destroy;

#else // not (defined(SHARED) && defined(ENGINE_MAIN))

#if defined(SHARED) && defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT extern
#endif

EXPORT ee_init_fn ee_init;
EXPORT ee_reload_fn ee_reload;
EXPORT ee_frame_fn ee_frame;
EXPORT ee_destroy_fn ee_destroy;

#endif // not (defined(SHARED) && defined(ENGINE_MAIN))
