#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <caresto/g_graphics.h>
#include <caresto/l_log.h>
#include <caresto/mm_memory_management.h>

// Create our game window
SDL_Window *create_sdl_window() {
    const char *title = "Hello";

    // 640x360 is the perfect res for pixel art games because it scales evenly
    // to all target resolutions.  We need to start the window at user's native
    // res though.  We use a 640x360 to paint the game, then scale it to native
    // res.  GUI should be painted on the native res surface.
    int width = 640;
    int height = 360;
    Uint32 flags = SDL_WINDOW_OPENGL;
    SDL_Window *sdl_window = SDL_CreateWindow(title, width, height, flags);
    return sdl_window;
}

#define MB_10 (10 * 1024 * 1024)

int main(int argc, char *argv[]) {
    int rc = 0;
    SDL_Window *sdl_window = NULL;
    unsigned char *buffer = NULL;
    struct g_program program = {0};
    struct g_texture texture = {0};
    SDL_GLContext sdl_gl_context = NULL;

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

    // Log level: DEBUG -- TODO: Change for release
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);

    // Allocate persistent storage
    buffer = (unsigned char *)mm_alloc(MB_10);
    if (buffer == NULL) {
        l_critical("OOM: Could not create arena.");
        rc = -1;
        goto _err;
    }

    struct mm_arena arena = mm_arena_create(MB_10, buffer);

    // Initialize SDL
    Uint32 sdl_flags = SDL_INIT_VIDEO;
    if (!SDL_Init(sdl_flags)) {
        const char *sdl_error = SDL_GetError();
        l_critical("SDL: Could not initialize. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Create our game window
    sdl_window = create_sdl_window();
    if (sdl_window == NULL) {
        const char *sdl_error = SDL_GetError();
        l_critical("SDL: Could not create window. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Request OpenGL version 4.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create a OpenGL Context
    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == NULL) {
        const char *sdl_error = SDL_GetError();
        l_critical("SDL: Could not create OpenGL context. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Initialize glew
    GLenum glew_rc = glewInit();
    if (GLEW_OK != glew_rc) {
        const GLubyte *glew_error = glewGetErrorString(glew_rc);
        l_critical("GL: Could not initialize glew. %s\n", glew_error);
        rc = -1;
        goto _err;
    }

    // Check what version of OpenGL we got
    int gl_major_version = 0;
    int gl_minor_version = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);
    l_debug("GL: Version %d.%d\n", gl_major_version, gl_minor_version);

    // Debug callback is only available in 4.3+
    if (gl_major_version > 4 ||
        (gl_major_version == 4 && gl_minor_version >= 3)) {
        // This always generates a GL_INVALID_ENUM
        // glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(g_debug_message_callback, NULL);
    }

    rc = g_program_create(&arena, &program);
    if (rc != 0) {
        goto _err;
    }

    // Set orthographic projection camera
    GLfloat screen_width = 640.0f;
    GLfloat screen_height = 360.0f;
    struct g_mat4 ortho = {.values = {0.0f}};
    g_ortho(&ortho, 0.0f, screen_width, 0.0f, screen_height, 0.0f, 1.0f);

    rc = g_texture_load("assets/sprite_atlas.png", &texture);
    if (rc != 0) {
        goto _err;
    }

    GLsizei object_count = 1;
    struct g_sprite sprites[] = {
        {.x = 0.0f, .y = 0.0f, .w = 16, .h = 16, .u = 0, .v = 0},
    };

    // TODO:
    // - Sprite atlas generator
    // - Render function for sprites

    // Main event loop
    bool running = true;
    SDL_Event sdl_event = {0};
    Uint64 last_tick = SDL_GetTicks();
    while (running) {
        Uint64 current_tick = SDL_GetTicks();
        Uint64 delta_time = current_tick - last_tick;
        last_tick = current_tick;

        // Update the VBO
        glBindBuffer(GL_ARRAY_BUFFER, program.buffer_id);
        sprites[0].x += delta_time / 100.0f;
        if (sprites[0].x > 100.0f) {
            sprites[0].x = 0.0f;
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprites), sprites);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the VAO
        glUseProgram(program.program_id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glBindVertexArray(program.vertex_array_id);
        glUniformMatrix4fv(program.g_transform_mat_id, 1, GL_FALSE,
                           ortho.values);
        glDrawArrays(GL_POINTS, 0, object_count);
        glBindVertexArray(0);
        glUseProgram(0);

        // Swap buffers
        SDL_GL_SwapWindow(sdl_window);

        // Handle input
        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (sdl_event.key.key == SDLK_Q) {
                    running = false;
                }
                break;
            }
        }
    }

    goto _done;

_err:
_done:
    if (sdl_gl_context != NULL) {
        SDL_GL_DestroyContext(sdl_gl_context);
    }
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    g_program_destroy(&program);
    g_texture_destroy(&texture);
    mm_free(buffer);
    return rc;
}
