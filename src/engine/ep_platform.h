#ifndef EP_PLATFORM_H
#define EP_PLATFORM_H

#ifdef SHARED

#include <stdbool.h>

#include <engine/egl_opengl.h>
#include <engine/em_memory.h>

#ifdef _WIN32

#include <windows.h>
typedef HMODULE ep_shared;

#endif // _WIN32

typedef int (*cg_init_fn)(void **, struct em_arena *, struct em_arena *);
typedef void (*cg_reload_fn)(void *, struct em_arena *);
typedef bool (*cg_frame_fn)(void *, struct egl_frame *);
typedef void (*cg_destroy_fn)(void *);

struct ep_shared_game {
    ep_shared shared_lib;
    long long timestamp;
    const char *path;
    bool is_red;
    cg_init_fn cg_init;
    cg_reload_fn cg_reload;
    cg_frame_fn cg_frame;
    cg_destroy_fn cg_destroy;
};

int ep_shared_load(const char *path, struct em_arena *arena,
                   struct ep_shared_game *out_shared_game);

bool ep_shared_reload(struct em_arena *arena,
                      struct ep_shared_game *out_shared_game);

void ep_shared_free(ep_shared shared);

#endif // SHARED

#endif // EP_PLATFORM_H
