#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <mm_memory_management.h>
#include <l_log.h>

// Create our game window
SDL_Window *create_sdl_window() {
    const char *title = "Hello";

    // 640x360 is the perfect res for pixel art games because it scales evenly to all
    // target resolutions.  We need to start the window at user's native res though.
    // We use a 640x360 to paint the game, then scale it to native res.  GUI should
    // be painted on the native res surface.
    int width = 640;
    int height = 360;
    Uint32 flags = SDL_WINDOW_OPENGL;
    SDL_Window *sdl_window = SDL_CreateWindow(title, width, height, flags);
    return sdl_window;
}

int main(int argc, char *argv[]) {
    int rc = 0;
    SDL_Window *sdl_window = NULL;
    mm_arena arena = mm_arena_zero;

    arena = mm_arena_create(1024);
    if (arena.size <= 0) {
        l_critical("Could not allocate memory.");
        rc = -1;
        goto _done;
    }

    // Initialize SDL
    Uint32 sdl_flags = SDL_INIT_VIDEO;
    if (!SDL_Init(sdl_flags)) {
        const char *sdl_error = SDL_GetError();
        l_critical("Could not initialize SDL. %s\n", sdl_error);
        rc = -1;
        goto _done;
    }

    // Create our game window
    sdl_window = create_sdl_window();
    if (sdl_window == NULL) {
        const char *sdl_error = SDL_GetError();
        l_critical("Could not create SDL window. %s\n", sdl_error);
        rc = -1;
        goto _done;
    }

    // Create a OpenGL Context
    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == NULL) {
        const char *sdl_error = SDL_GetError();
        l_critical("Could not create OpenGL context. %s\n", sdl_error);
        rc = -1;
        goto _done;
    }

    // Initialize glew
    GLenum glew_rc = glewInit();
    if (GLEW_OK != glew_rc) {
        const GLubyte *glew_error = glewGetErrorString(glew_rc);
        l_critical("Could not initialize glew. %s\n", glew_error);
        rc = -1;
        goto _done;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // TODO:
    // - create gl program
    // - create gl vertex shader
    // - create gl fragment shader
    // - create gl vbo
    // - remember to add gl cleanup code
    // - draw something to screen

    // Main event loop
    bool running = true;
    SDL_Event sdl_event = {0};
    while (running) {
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
        SDL_GL_SwapWindow(sdl_window);
    }

    // We're done
_done:
    mm_arena_destroy(&arena);
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    return rc;
}

