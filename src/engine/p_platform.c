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
    // +2 ("-b") +1 (NULL terminator)
    size_t new_path_len = strlen(path) + 2 + 1;
    char *new_path = (char *)mm_arena_alloc(arena, sizeof(char) * new_path_len);
    if (new_path == NULL) {
        l_critical("WIN: OOM: Could not allocate to write file %s.\n", path);
        rc = -1;
        goto _err;
    }
    if (out_shared_game->is_red) {
        snprintf(new_path, new_path_len, "%s-b", path);
    } else {
        snprintf(new_path, new_path_len, "%s-r", path);
    }

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
    out_shared_game->path = path;
    out_shared_game->shared_lib = dll;
    out_shared_game->timestamp = timestamp;
    out_shared_game->is_red = !out_shared_game->is_red;
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

bool p_shared_reload(struct mm_arena *arena,
                     struct p_shared_game *out_shared_game) {
    struct stat file_stat = {0};
    stat(out_shared_game->path, &file_stat);
    long long timestamp = file_stat.st_mtime;

    // DLL is up to date, no reload
    if (timestamp <= out_shared_game->timestamp) {
        return false;
    }

    // Try load the new DLL, if can't, bail out and continue as nothing happened
    struct p_shared_game new_shared_game = {.is_red = out_shared_game->is_red};
    int rc = p_shared_load(out_shared_game->path, arena, &new_shared_game);
    if (rc != 0) {
        return false;
    }

    // Release old lib
    FreeLibrary(out_shared_game->shared_lib);

    out_shared_game->path = new_shared_game.path;
    out_shared_game->shared_lib = new_shared_game.shared_lib;
    out_shared_game->timestamp = new_shared_game.timestamp;
    out_shared_game->is_red = new_shared_game.is_red;
    out_shared_game->g_init = new_shared_game.g_init;
    out_shared_game->g_process_frame = new_shared_game.g_process_frame;
    return true;
}

void p_shared_free(p_shared shared) { FreeLibrary(shared); }

#endif // _WIN32

#endif // SHARED
