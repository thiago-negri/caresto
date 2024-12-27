#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <GL/glew.h>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

// Create our game window
SDL_Window *create_sdl_window() {
    const char *title = "Hello";
    int x = SDL_WINDOWPOS_UNDEFINED;
    int y = SDL_WINDOWPOS_UNDEFINED;
    // 640x360 is the perfect res for pixel art games because it scales evenly to all
    // target resolutions.  We need to start the window at user's native res though.
    // We use a 640x360 to paint the game, then scale it to native res.  GUI should
    // be painted on the native res surface.
    int width = 640;
    int height = 360;
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    SDL_Window *sdl_window = SDL_CreateWindow(title, x, y, width, height, flags);
    return sdl_window;
}

int main(int argc, char *argv[]) {
    int rc = 0;
    SDL_Window *sdl_window = NULL;

    // Initialize SDL
    Uint32 sdl_flags = SDL_INIT_VIDEO;
    int sdl_rc = SDL_Init(sdl_flags);
    if (sdl_rc != 0) {
        const char *sdl_error = SDL_GetError();
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL. %s\n", sdl_error);
        rc = -1;
        goto _done;
    }

    // Create our game window
    sdl_window = create_sdl_window();
    if (sdl_window == NULL) {
        const char *sdl_error = SDL_GetError();
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not create SDL window. %s\n", sdl_error);
        rc = -1;
        goto _done;
    }

    // Create a OpenGL Context
    SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == NULL) {
        const char *sdl_error = SDL_GetError();
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not create OpenGL context. %s\n",
                sdl_error);
        rc = -1;
        goto _done;
    }

    // Initialize glew
    GLenum glew_rc = glewInit();
    if (GLEW_OK != glew_rc) {
        const GLubyte *glew_error = glewGetErrorString(glew_rc);
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize glew. %s\n", glew_error);
        rc = -1;
        goto _done;
    }

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
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (sdl_event.key.keysym.sym == SDLK_q) {
                        running = false;
                    }
                    break;
            }

            SDL_GL_SwapWindow(sdl_window);
        }
    }

    // We're done
_done:
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    return rc;
}

