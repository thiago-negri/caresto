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

    // Set OpenGL version to at least 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create a OpenGL Context
    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_window);
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

    rc = g_program_create(&arena, &program);
    if (rc != 0) {
        goto _err;
    }

    GLsizei object_count = 2;
    struct g_sprite sprites[] = {
        { .x = 0.5f, .y = 0.5f, .w = 0.1f, .h = 0.1f },
        { .x = 0.0f, .y = 0.0f, .w = 0.3f, .h = 0.3f },
    };

    // TODO:
    // - texture
    // - camera

    // Main event loop
    bool running = true;
    SDL_Event sdl_event = {0};
    Uint64 last_tick = SDL_GetTicks();
    while (running) {
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

        Uint64 current_tick = SDL_GetTicks();
        Uint64 delta_time = current_tick - last_tick;
        last_tick = current_tick;

        // Update the VBO
        glBindBuffer(GL_ARRAY_BUFFER, program.buffer_id);
        sprites[0].x += delta_time / 5000.0f;
        if (sprites[0].x > 0.8f) {
            sprites[0].x = -0.8f;
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprites), sprites);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the VAO
        glUseProgram(program.program_id);
        glBindVertexArray(program.vertex_array_id);
        glDrawArrays(GL_POINTS, 0, object_count);

        // Reset context (not really required)
        glBindVertexArray(0);
        glUseProgram(0);

        // Swap buffers
        SDL_GL_SwapWindow(sdl_window);
    }

    goto _done;

_err:
_done:
    g_program_destroy(&program);
    mm_free(buffer);
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    return rc;
}
