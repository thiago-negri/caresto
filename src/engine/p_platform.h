#ifndef P_PLATFORM_H
#define P_PLATFORM_H

#ifdef SHARED

#include <stdbool.h>

#include <engine/gl_opengl.h>
#include <engine/mm_memory_management.h>

typedef void *(*g_init_fn)(struct mm_arena *);
typedef bool (*g_process_frame_fn)(struct gl_frame *, void *);

struct p_shared_game {
    g_init_fn g_init;
    g_process_frame_fn g_process_frame;
};

#ifdef _WIN32

#include <windows.h>
typedef HMODULE p_shared;

#endif // _WIN32

p_shared p_shared_load(const char *path, struct p_shared_game *out_shared_game);

void p_shared_free(p_shared shared);

#endif // SHARED

#endif // P_PLATFORM_H
