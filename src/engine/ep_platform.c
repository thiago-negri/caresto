#include <engine/ep_platform.h>

#if defined(SHARED)

#ifdef _WIN32

#include <engine/ef_file.h>
#include <engine/el_log.h>
#include <stdio.h>

int ep_shared_load(struct ep_shared_game *out_shared_game, const char *path,
                   struct ea_arena *arena) {
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

    ee_init_fn *ee_init_ptr = (ee_init_fn *)GetProcAddress(dll, "ee_init");
    if (ee_init_ptr == NULL) {
        el_critical("WIN: Could not find ee_init.\n");
        rc = -1;
        goto _err;
    }

    ee_reload_fn *ee_reload_ptr =
        (ee_reload_fn *)GetProcAddress(dll, "ee_reload");
    if (ee_reload_ptr == NULL) {
        el_critical("WIN: Could not find ee_reload.\n");
        rc = -1;
        goto _err;
    }

    ee_frame_fn *ee_frame_ptr = (ee_frame_fn *)GetProcAddress(dll, "ee_frame");
    if (ee_frame_ptr == NULL) {
        el_critical("WIN: Could not find ee_frame.\n");
        rc = -1;
        goto _err;
    }

    ee_destroy_fn *ee_destroy_ptr =
        (ee_destroy_fn *)GetProcAddress(dll, "ee_destroy");
    if (ee_frame_ptr == NULL) {
        el_critical("WIN: Could not find ee_destroy.\n");
        rc = -1;
        goto _err;
    }

    el_debug("WIN: Loaded caresto.dll.\n");
    out_shared_game->path = path;
    out_shared_game->shared_lib = dll;
    out_shared_game->timestamp = timestamp;
    out_shared_game->is_red = !out_shared_game->is_red;
    out_shared_game->ee_init = ee_init_ptr;
    out_shared_game->ee_reload = ee_reload_ptr;
    out_shared_game->ee_frame = ee_frame_ptr;
    out_shared_game->ee_destroy = ee_destroy_ptr;
    goto _done;

_err:
    if (dll != NULL) {
        FreeLibrary(dll);
    }

_done:
    ea_arena_restore_offset(arena, arena_offset);
    return rc;
}

bool ep_shared_reload(struct ep_shared_game *out_shared_game,
                      struct ea_arena *arena) {
    long long timestamp = ef_timestamp(out_shared_game->path);

    // DLL is up to date, no reload
    if (timestamp <= out_shared_game->timestamp) {
        return false;
    }

    ep_shared old_lib = out_shared_game->shared_lib;

    // Try load the new DLL, if can't, bail out and continue as nothing happened
    int rc = ep_shared_load(out_shared_game, out_shared_game->path, arena);
    if (rc != 0) {
        return false;
    }

    // Release old lib
    FreeLibrary(old_lib);
    return true;
}

void ep_shared_free(struct ep_shared_game *out_shared_game) {
    FreeLibrary(out_shared_game->shared_lib);
}

#endif // _WIN32

#endif // defined(SHARED)
