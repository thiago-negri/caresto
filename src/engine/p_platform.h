#ifndef P_PLATFORM_H
#define P_PLATFORM_H

#ifdef SHARED

#include <stdbool.h>

#include <engine/gl_opengl.h>
#include <engine/mm_memory_management.h>

#ifdef _WIN32

#include <windows.h>
typedef HMODULE p_shared;

#endif // _WIN32

typedef void *(*g_init_fn)(struct mm_arena *);
typedef bool (*g_process_frame_fn)(struct gl_frame *, void *);

struct p_shared_game {
    p_shared shared_lib;
    long long timestamp;
    const char *path;
    bool is_red;
    g_init_fn g_init;
    g_process_frame_fn g_process_frame;
};

int p_shared_load(const char *path, struct mm_arena *arena,
                  struct p_shared_game *out_shared_game);

bool p_shared_reload(struct mm_arena *arena,
                     struct p_shared_game *out_shared_game);

void p_shared_free(p_shared shared);

#endif // SHARED

#endif // P_PLATFORM_H
