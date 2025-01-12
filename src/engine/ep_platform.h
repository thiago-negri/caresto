#pragma once

#if defined(SHARED)

#include <engine/ea_allocator.h>
#include <engine/ee_entry.h>

#ifdef _WIN32

#include <Windows.h>
typedef HMODULE ep_shared;

#endif // _WIN32

struct ep_shared_game {
    ep_shared shared_lib;
    long long timestamp;
    const char *path;
    bool is_red;
    ee_init_fn *ee_init;
    ee_reload_fn *ee_reload;
    ee_frame_fn *ee_frame;
    ee_destroy_fn *ee_destroy;
};

int ep_shared_load(struct ep_shared_game *out_shared_game, const char *path,
                   struct ea_arena *transient);

bool ep_shared_reload(struct ep_shared_game *out_shared_game,
                      struct ea_arena *transient);

void ep_shared_free(struct ep_shared_game *shared);

#endif // defined(SHARED)
