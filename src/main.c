#define ENGINE_MAIN

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <engine/ea_allocator.h>
#include <engine/ee_entry.h>
#include <engine/el_log.h>
#include <engine/eo_opengl.h>
#include <engine/ep_platform.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define APP_NAME "Caresto"
#define APP_VERSION "0.0.1-dev"
#define APP_ID "com.riverstone.caresto"
#define APP_CREATOR "River Stone Games"
#define APP_COPYRIGHT "Copyright (c) River Stone Games"
#define APP_TYPE "game"
#define APP_URL "TODO: Steam URL"

#define SHARED_LIB_PATH "build/debug/bin/caresto.dll"
#define SHARED_LIB_CHECK_INTERVAL_MS 5000.0f

#define MB_10 (10 * 1024 * 1024)
#define MB_20 (20 * 1024 * 1024)

#define WINDOW_W 1280
#define WINDOW_H 720

// Create our game window
static SDL_Window *create_sdl_window() {
    const char *title = "Caresto";
    Uint32 flags = SDL_WINDOW_OPENGL;
    SDL_Window *sdl_window = SDL_CreateWindow(title, WINDOW_W, WINDOW_H, flags);

#ifdef DEBUG
    SDL_SetWindowPosition(sdl_window, 0, 30);
#endif

    return sdl_window;
}

int main(int /*argc*/, char * /*argv*/[]) {
    int rc = 0;
    unsigned char *memory_buffer = NULL;
    SDL_Window *sdl_window = NULL;
    SDL_GLContext sdl_gl_context = NULL;
    void *game_data = NULL;
#ifdef SHARED
    ep_shared shared = NULL;
#endif // SHARED

    // App Metadata
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, APP_NAME);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING,
                               APP_VERSION);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, APP_ID);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING,
                               APP_CREATOR);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING,
                               APP_COPYRIGHT);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, APP_TYPE);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, APP_URL);

#ifdef DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif // DEBUG

    // Allocate persistent storage
    memory_buffer = (unsigned char *)ea_alloc(MB_20);
    if (memory_buffer == NULL) {
        el_critical("OOM: Could not allocate memory.");
        rc = -1;
        goto _err;
    }

    struct ea_arena persistent_storage = {};
    struct ea_arena transient_storage = {};
    ea_arena_init(&persistent_storage, MB_10, memory_buffer);
    ea_arena_init(&transient_storage, MB_10, memory_buffer + MB_10);

#ifdef SHARED
    struct ep_shared_game shared_game = {0};
    rc = ep_shared_load(&shared_game, SHARED_LIB_PATH, &transient_storage);
    if (rc != 0) {
        goto _err;
    }
    ee_init = shared_game.ee_init;
    ee_reload = shared_game.ee_reload;
    ee_frame = shared_game.ee_frame;
    ee_destroy = shared_game.ee_destroy;
#endif // SHARED

    // Initialize SDL
    Uint32 sdl_flags = SDL_INIT_VIDEO;
    if (!SDL_Init(sdl_flags)) {
        const char *sdl_error = SDL_GetError();
        el_critical_fmt("SDL: Could not initialize. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Create our game window
    sdl_window = create_sdl_window();
    if (sdl_window == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical_fmt("SDL: Could not create window. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Request OpenGL version 4.3
    /*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);*/
    /*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);*/
    /*SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,*/
    /*                    SDL_GL_CONTEXT_PROFILE_CORE);*/
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // Create a OpenGL Context
    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical_fmt("SDL: Could not create OpenGL context. %s\n",
                        sdl_error);
        rc = -1;
        goto _err;
    }

    // Initialize glew
    GLenum glew_rc = glewInit();
    if (GLEW_OK != glew_rc) {
        const GLubyte *glew_error = glewGetErrorString(glew_rc);
        el_critical_fmt("GL: Could not initialize glew. %s\n", glew_error);
        rc = -1;
        goto _err;
    }

    // Check what version of OpenGL we got
    int gl_major_version = 0;
    int gl_minor_version = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);
    el_debug_fmt("GL: Version %d.%d\n", gl_major_version, gl_minor_version);

    // Debug callback is only available in 4.3+
    if (gl_major_version > 4 ||
        (gl_major_version == 4 && gl_minor_version >= 3)) {
        glDebugMessageCallback(eo_debug_message_callback, NULL);
    }

    // Initialize game
    rc = ee_init(&game_data, &persistent_storage, &transient_storage);
    if (rc != 0) {
        goto _err;
    }

#ifdef SHARED
    double check_shared_lib_timeout = SHARED_LIB_CHECK_INTERVAL_MS;
#endif // SHARED

    // Main event loop
    bool running = true;
    Uint64 last_perf_counter = SDL_GetPerformanceCounter();
    while (running) {
        Uint64 current_perf_counter = SDL_GetPerformanceCounter();
        double delta_time = ((current_perf_counter - last_perf_counter) *
                             1000.0f / SDL_GetPerformanceFrequency());
        last_perf_counter = current_perf_counter;

#ifdef SHARED
        check_shared_lib_timeout -= delta_time;
        if (check_shared_lib_timeout < 0.0f) {
            check_shared_lib_timeout = SHARED_LIB_CHECK_INTERVAL_MS;
            bool reloaded = ep_shared_reload(&shared_game, &transient_storage);
            if (reloaded) {
                ee_init = shared_game.ee_init;
                ee_reload = shared_game.ee_reload;
                ee_frame = shared_game.ee_frame;
                ee_destroy = shared_game.ee_destroy;
            }
            // Call reload even if the same DLL is still loaded,
            // this way the game can reload external assets (e.g. sprite atlas)
            ee_reload(game_data, &transient_storage);
        }
#endif // SHARED

        // Process game frame, game is responsible for writing to
        // the current OpenGL buffer
        struct eo_frame frame = {.sdl_window = sdl_window,
                                 .delta_time = delta_time};
        running = ee_frame(game_data, &frame);

        // Swap buffers
        SDL_GL_SwapWindow(sdl_window);

        ea_arena_reset(&transient_storage);
    }

    goto _done;

_err:
_done:
    ee_destroy(game_data);
    if (sdl_gl_context != NULL) {
        SDL_GL_DestroyContext(sdl_gl_context);
    }
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    ea_free(memory_buffer);
#ifdef SHARED
    if (shared != NULL) {
        ep_shared_free(&shared_game);
    }
#endif // SHARED
    return rc;
}
