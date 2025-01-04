#include <engine/p_platform.h>

#ifdef SHARED

#ifdef _WIN32

#include <stdio.h>
#include <sys/stat.h>

#include <engine/l_log.h>
#include <engine/u_utils.h>

int p_shared_load(const char *path, struct mm_arena *arena,
                  struct p_shared_game *out_shared_game) {
    int rc = 0;
    HMODULE dll = NULL;

    size_t arena_offset = mm_arena_save_offset(arena);

    // Save DLL timestamp, we will use this to check whether we need to
    // reload it
    struct stat file_stat = {0};
    stat(path, &file_stat);
    long long timestamp = file_stat.st_mtime;

    // Copy DLL to a new location to avoid locking the DLL we build
    // +5 ("-load") +1 (NULL terminator)
    size_t new_path_len = strlen(path) + 5 + 1;
    char *new_path = (char *)mm_arena_alloc(arena, sizeof(char) * new_path_len);
    if (new_path == NULL) {
        l_critical("WIN: OOM: Could not allocate to write file %s.\n",
                        path);
        rc = -1;
        goto _err;
    }
    snprintf(new_path, new_path_len, "%s-load", path);

    rc = u_copy_file(path, new_path, arena);
    if (rc != 0) {
        goto _err;
    }

    // Load DLL from the new location
    dll = LoadLibraryA(new_path);
    if (dll == NULL) {
        l_critical("WIN: Could not load DLL %s.\n", path);
        rc = -1;
        goto _err;
    }

    g_init_fn g_init_ptr = (g_init_fn)GetProcAddress(dll, "g_init");
    if (g_init_ptr == NULL) {
        l_critical("WIN: Could not find g_init.\n");
        rc = -1;
        goto _err;
    }

    g_process_frame_fn g_process_frame_ptr =
        (g_process_frame_fn)GetProcAddress(dll, "g_process_frame");
    if (g_process_frame_ptr == NULL) {
        l_critical("WIN: Could not find g_process_frame.\n");
        rc = -1;
        goto _err;
    }

    l_debug("WIN: Loaded caresto.dll.\n");
    out_shared_game->shared_lib = dll;
    out_shared_game->timestamp = timestamp;
    out_shared_game->g_init = g_init_ptr;
    out_shared_game->g_process_frame = g_process_frame_ptr;
    goto _done;

_err:
    if (dll != NULL) {
        FreeLibrary(dll);
    }

_done:
    mm_arena_restore_offset(arena, arena_offset);
    return rc;
}

void p_shared_free(p_shared shared) { FreeLibrary(shared); }

#endif // _WIN32

#endif // SHARED
