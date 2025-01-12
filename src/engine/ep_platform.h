#pragma once

#ifdef SHARED

#include <engine/ea_allocator.h>
#include <engine/eo_opengl.h>

#ifdef _WIN32

#include <Windows.h>
typedef HMODULE ep_shared;

#endif // _WIN32

typedef int (*cgl_init_fn)(void **, struct ea_arena *, struct ea_arena *);
typedef void (*cgl_reload_fn)(void *, struct ea_arena *);
typedef bool (*cgl_frame_fn)(void *, struct eo_frame *);
typedef void (*cgl_destroy_fn)(void *);

struct ep_shared_game {
    ep_shared shared_lib;
    long long timestamp;
    const char *path;
    bool is_red;
    cgl_init_fn cgl_init;
    cgl_reload_fn cgl_reload;
    cgl_frame_fn cgl_frame;
    cgl_destroy_fn cgl_destroy;
};

int ep_shared_load(const char *path, struct ea_arena *arena,
                   struct ep_shared_game *out_shared_game);

bool ep_shared_reload(struct ea_arena *arena,
                      struct ep_shared_game *out_shared_game);

void ep_shared_free(ep_shared shared);

#endif // SHARED
