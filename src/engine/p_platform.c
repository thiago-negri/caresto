#include <engine/p_platform.h>

#ifdef SHARED

#ifdef _WIN32

#include <engine/l_log.h>

p_shared p_shared_load(const char *path,
                       struct p_shared_game *out_shared_game) {
    HMODULE dll = LoadLibraryA("caresto.dll");
    if (dll == NULL) {
        l_critical("WIN: Could not load caresto.dll.\n");
        goto _err;
    }

    g_init_fn g_init_ptr = (g_init_fn)GetProcAddress(dll, "g_init");
    if (g_init_ptr == NULL) {
        l_critical("WIN: Could not find g_init.\n");
        goto _err;
    }

    g_process_frame_fn g_process_frame_ptr =
        (g_process_frame_fn)GetProcAddress(dll, "g_process_frame");
    if (g_process_frame_ptr == NULL) {
        l_critical("WIN: Could not find g_process_frame.\n");
        goto _err;
    }

    l_debug("WIN: Loaded caresto.dll.\n");
    out_shared_game->g_init = g_init_ptr;
    out_shared_game->g_process_frame = g_process_frame_ptr;
    goto _done;

_err:
    if (dll != NULL) {
        FreeLibrary(dll);
    }
    g_init_ptr = NULL;
    g_process_frame_ptr = NULL;
    return NULL;

_done:
    return dll;
}

void p_shared_free(p_shared shared) { FreeLibrary(shared); }

#endif // _WIN32

#endif // SHARED
