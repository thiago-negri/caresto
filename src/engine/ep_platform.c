#include <engine/ep_platform.h>

#ifdef SHARED

#ifdef _WIN32

#include <engine/ef_file.h>
#include <engine/el_log.h>
#include <stdio.h>

int ep_shared_load(const char *path, struct ea_arena *arena,
                   struct ep_shared_game *out_shared_game) {
    int rc = 0;
    HMODULE dll = NULL;

    size_t arena_offset = ea_arena_save_offset(arena);

    // Save DLL timestamp, we will use this to check whether we need to
    // reload it
    long long timestamp = ef_timestamp(path);

    // Copy DLL to a new location to avoid locking the DLL we build
    // +2 ("-b") +1 (NULL terminator)
    size_t new_path_len = strlen(path) + 2 + 1;
    char *new_path = (char *)ea_arena_alloc(arena, sizeof(char) * new_path_len);
    if (new_path == NULL) {
        el_critical_fmt("WIN: OOM: Could not allocate to write file %s.\n",
                        path);
        rc = -1;
        goto _err;
    }
    if (out_shared_game->is_red) {
        snprintf(new_path, new_path_len, "%s-b", path);
    } else {
        snprintf(new_path, new_path_len, "%s-r", path);
    }

    rc = ef_copy(new_path, path, arena);
    if (rc != 0) {
        goto _err;
    }

    // Load DLL from the new location
    dll = LoadLibraryA(new_path);
    if (dll == NULL) {
        el_critical_fmt("WIN: Could not load DLL %s.\n", path);
        rc = -1;
        goto _err;
    }

    cgl_init_fn cgl_init_ptr = (cgl_init_fn)GetProcAddress(dll, "cgl_init");
    if (cgl_init_ptr == NULL) {
        el_critical("WIN: Could not find cgl_init.\n");
        rc = -1;
        goto _err;
    }

    cgl_reload_fn cgl_reload_ptr =
        (cgl_reload_fn)GetProcAddress(dll, "cgl_reload");
    if (cgl_reload_ptr == NULL) {
        el_critical("WIN: Could not find cgl_reload.\n");
        rc = -1;
        goto _err;
    }

    cgl_frame_fn cgl_frame_ptr = (cgl_frame_fn)GetProcAddress(dll, "cgl_frame");
    if (cgl_frame_ptr == NULL) {
        el_critical("WIN: Could not find cgl_frame.\n");
        rc = -1;
        goto _err;
    }

    cgl_destroy_fn cgl_destroy_ptr =
        (cgl_destroy_fn)GetProcAddress(dll, "cgl_destroy");
    if (cgl_frame_ptr == NULL) {
        el_critical("WIN: Could not find cgl_destroy.\n");
        rc = -1;
        goto _err;
    }

    el_debug("WIN: Loaded caresto.dll.\n");
    out_shared_game->path = path;
    out_shared_game->shared_lib = dll;
    out_shared_game->timestamp = timestamp;
    out_shared_game->is_red = !out_shared_game->is_red;
    out_shared_game->cgl_init = cgl_init_ptr;
    out_shared_game->cgl_reload = cgl_reload_ptr;
    out_shared_game->cgl_frame = cgl_frame_ptr;
    out_shared_game->cgl_destroy = cgl_destroy_ptr;
    goto _done;

_err:
    if (dll != NULL) {
        FreeLibrary(dll);
    }

_done:
    ea_arena_restore_offset(arena, arena_offset);
    return rc;
}

bool ep_shared_reload(struct ea_arena *arena,
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
    out_shared_game->cgl_init = new_shared_game.cgl_init;
    out_shared_game->cgl_reload = new_shared_game.cgl_reload;
    out_shared_game->cgl_frame = new_shared_game.cgl_frame;
    out_shared_game->cgl_destroy = new_shared_game.cgl_destroy;
    return true;
}

void ep_shared_free(ep_shared shared) { FreeLibrary(shared); }

#endif // _WIN32

#endif // SHARED
