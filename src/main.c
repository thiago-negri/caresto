#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/em_memory.h>
#include <engine/ep_platform.h>

#define CARESTO_MAIN
#include <caresto/cg_game.h>

#define MB_10 (10 * 1024 * 1024)
#define MB_20 (20 * 1024 * 1024)

// FIXME(tnegri): SPRITE_MAX is shared between main and cg_game
#define SPRITE_MAX 1024

// Create our game window
SDL_Window *create_sdl_window() {
    const char *title = "Hello";
    int width = 1280;
    int height = 720;
    Uint32 flags = SDL_WINDOW_OPENGL;
    SDL_Window *sdl_window = SDL_CreateWindow(title, width, height, flags);

#ifdef DEBUG
    SDL_SetWindowPosition(sdl_window, 1250, 40);
#endif

    return sdl_window;
}

int main(int argc, char *argv[]) {
    int rc = 0;
    SDL_Window *sdl_window = NULL;
    unsigned char *buffer = NULL;
    struct egl_program program = {0};
    struct egl_texture sprite_atlas = {0};
    SDL_GLContext sdl_gl_context = NULL;
    struct egl_sprite_buffer sprite_buffer = {0};
#ifdef SHARED
    ep_shared shared = NULL;
#endif

    // App Metadata
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "Caresto");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING,
                               "0.0.1-dev");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING,
                               "com.riverstonegames.caresto");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING,
                               "River Stone Games");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING,
                               "Copyright (c) River Stone Games LTDA ME");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING,
                               "TODO: steam_url");

#ifdef DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif

    // Allocate persistent storage
    buffer = (unsigned char *)em_alloc(MB_20);
    if (buffer == NULL) {
        el_critical("OOM: Could not allocate memory.");
        rc = -1;
        goto _err;
    }

    struct em_arena persistent_storage = em_arena_create(MB_10, buffer);
    struct em_arena transient_storage = em_arena_create(MB_10, buffer + MB_10);

#ifdef SHARED
    struct ep_shared_game shared_game = {0};
    rc = ep_shared_load("build/debug/bin/caresto.dll", &transient_storage,
                        &shared_game);
    if (rc != 0) {
        goto _err;
    }
    cg_init_ptr = shared_game.cg_init;
    cg_process_frame_ptr = shared_game.cg_process_frame;
#endif

    // Initialize SDL
    Uint32 sdl_flags = SDL_INIT_VIDEO;
    if (!SDL_Init(sdl_flags)) {
        const char *sdl_error = SDL_GetError();
        el_critical("SDL: Could not initialize. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Create our game window
    sdl_window = create_sdl_window();
    if (sdl_window == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical("SDL: Could not create window. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Request OpenGL version 4.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // Create a OpenGL Context
    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical("SDL: Could not create OpenGL context. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Initialize glew
    GLenum glew_rc = glewInit();
    if (GLEW_OK != glew_rc) {
        const GLubyte *glew_error = glewGetErrorString(glew_rc);
        el_critical("GL: Could not initialize glew. %s\n", glew_error);
        rc = -1;
        goto _err;
    }

    // Check what version of OpenGL we got
    int gl_major_version = 0;
    int gl_minor_version = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);
    el_debug("GL: Version %d.%d\n", gl_major_version, gl_minor_version);

    // Debug callback is only available in 4.3+
    if (gl_major_version > 4 ||
        (gl_major_version == 4 && gl_minor_version >= 3)) {
        glDebugMessageCallback(egl_debug_message_callback, NULL);
    }

    rc = egl_program_create(&transient_storage, &program);
    if (rc != 0) {
        goto _err;
    }

    egl_sprite_buffer_create(SPRITE_MAX, &sprite_buffer);

    // Set orthographic projection camera
    // 640x360 is the perfect res for pixel art games because it scales evenly
    // to all target resolutions.  We need to start the window at user's native
    // res though.  We use a 640x360 to paint the game, then scale it to native
    // res.  GUI should be painted on the native res surface.
    GLfloat screen_width = 640.0f;
    GLfloat screen_height = 360.0f;
    struct egl_mat4 camera_transform = {.values = {0.0f}};
    egl_ortho(&camera_transform, 0.0f, screen_width, 0.0f, screen_height, 0.0f,
              1.0f);

    rc = egl_texture_load("assets/sprite_atlas.png", &sprite_atlas);
    if (rc != 0) {
        goto _err;
    }

    // Initialize game
    void *game_data = cg_init(&persistent_storage);

#if SHARED
    Uint64 last_shared_lib_check = SDL_GetTicks();
#endif // SHARED

    // Main event loop
    bool running = true;
    Uint64 last_tick = SDL_GetTicks();
    while (running) {
        Uint64 current_tick = SDL_GetTicks();
        Uint64 delta_time = current_tick - last_tick;
        last_tick = current_tick;

#ifdef SHARED
        if (current_tick - last_shared_lib_check > 5000) {
            last_shared_lib_check = current_tick;
            bool reloaded = ep_shared_reload(&transient_storage, &shared_game);
            if (reloaded) {
                cg_init_ptr = shared_game.cg_init;
                cg_process_frame_ptr = shared_game.cg_process_frame;
            }
        }
#endif

        // Process game frame, game is responsible for writing to
        // the current OpenGL buffer
        struct egl_frame frame = {
            .delta_time = delta_time,
            .sprite_buffer = &sprite_buffer,
            .program = &program,
            .camera_transform = &camera_transform,
            .sprite_atlas = &sprite_atlas,
        };
        running = cg_process_frame(&frame, game_data);

        // Swap buffers
        SDL_GL_SwapWindow(sdl_window);

        em_arena_reset(&transient_storage);
    }

    goto _done;

_err:
_done:
    egl_sprite_buffer_destroy(&sprite_buffer);
    egl_program_destroy(&program);
    egl_texture_destroy(&sprite_atlas);
    if (sdl_gl_context != NULL) {
        SDL_GL_DestroyContext(sdl_gl_context);
    }
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    em_free(buffer);
#if DEBUG
    if (shared != NULL) {
        ep_shared_free(shared);
    }
#endif
    return rc;
}
