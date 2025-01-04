#include <engine/ep_platform.h>

#ifdef SHARED

#ifdef _WIN32

#include <stdio.h>

#include <engine/el_log.h>
#include <engine/eu_utils.h>

int ep_shared_load(const char *path, struct em_arena *arena,
                   struct ep_shared_game *out_shared_game) {
    int rc = 0;
    HMODULE dll = NULL;

    size_t arena_offset = em_arena_save_offset(arena);

    // Save DLL timestamp, we will use this to check whether we need to
    // reload it
    long long timestamp = eu_file_timestamp(path);

    // Copy DLL to a new location to avoid locking the DLL we build
    // +2 ("-b") +1 (NULL terminator)
    size_t new_path_len = strlen(path) + 2 + 1;
    char *new_path = (char *)em_arena_alloc(arena, sizeof(char) * new_path_len);
    if (new_path == NULL) {
        el_critical("WIN: OOM: Could not allocate to write file %s.\n", path);
        rc = -1;
        goto _err;
    }
    if (out_shared_game->is_red) {
        snprintf(new_path, new_path_len, "%s-b", path);
    } else {
        snprintf(new_path, new_path_len, "%s-r", path);
    }

    rc = eu_copy_file(path, new_path, arena);
    if (rc != 0) {
        goto _err;
    }

    // Load DLL from the new location
    dll = LoadLibraryA(new_path);
    if (dll == NULL) {
        el_critical("WIN: Could not load DLL %s.\n", path);
        rc = -1;
        goto _err;
    }

    cg_init_fn g_init_ptr = (cg_init_fn)GetProcAddress(dll, "cg_init");
    if (g_init_ptr == NULL) {
        el_critical("WIN: Could not find cg_init.\n");
        rc = -1;
        goto _err;
    }

    cg_process_frame_fn g_process_frame_ptr =
        (cg_process_frame_fn)GetProcAddress(dll, "cg_process_frame");
    if (g_process_frame_ptr == NULL) {
        el_critical("WIN: Could not find cg_process_frame.\n");
        rc = -1;
        goto _err;
    }

    el_debug("WIN: Loaded caresto.dll.\n");
    out_shared_game->path = path;
    out_shared_game->shared_lib = dll;
    out_shared_game->timestamp = timestamp;
    out_shared_game->is_red = !out_shared_game->is_red;
    out_shared_game->cg_init = g_init_ptr;
    out_shared_game->cg_process_frame = g_process_frame_ptr;
    goto _done;

_err:
    if (dll != NULL) {
        FreeLibrary(dll);
    }

_done:
    em_arena_restore_offset(arena, arena_offset);
    return rc;
}

bool ep_shared_reload(struct em_arena *arena,
                      struct ep_shared_game *out_shared_game) {
    struct stat file_stat = {0};
    stat(out_shared_game->path, &file_stat);
    long long timestamp = file_stat.st_mtime;

    // DLL is up to date, no reload
    if (timestamp <= out_shared_game->timestamp) {
        return false;
    }

    // Try load the new DLL, if can't, bail out and continue as nothing happened
    struct ep_shared_game new_shared_game = {.is_red = out_shared_game->is_red};
    int rc = ep_shared_load(out_shared_game->path, arena, &new_shared_game);
    if (rc != 0) {
        return false;
    }

    // Release old lib
    FreeLibrary(out_shared_game->shared_lib);

    out_shared_game->path = new_shared_game.path;
    out_shared_game->shared_lib = new_shared_game.shared_lib;
    out_shared_game->timestamp = new_shared_game.timestamp;
    out_shared_game->is_red = new_shared_game.is_red;
    out_shared_game->cg_init = new_shared_game.cg_init;
    out_shared_game->cg_process_frame = new_shared_game.cg_process_frame;
    return true;
}

void ep_shared_free(ep_shared shared) { FreeLibrary(shared); }

#endif // _WIN32

#endif // SHARED
