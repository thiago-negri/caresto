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
    GLuint program_id = 0;
    GLuint shader_vertex_id = 0;
    GLuint shader_fragment_id = 0;

    arena = mm_arena_create(10 * 1024 * 1024); // 10 MB
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

    shader_vertex_id = glCreateShader(GL_VERTEX_SHADER);
    {
        const char *shader_vertex_source = "#version 330 core\nvoid main() {}";
        glShaderSource(shader_vertex_id, 1, &shader_vertex_source, NULL);
        glCompileShader(shader_vertex_id);
        GLint compile_status = 0;
        glGetShaderiv(shader_vertex_id, GL_COMPILE_STATUS, &compile_status);
        if (compile_status != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(shader_vertex_id, GL_INFO_LOG_LENGTH, &log_length);
            char *log = mm_arena_alloc(&arena, log_length + 1);
            if (log == NULL) {
                l_critical("Shader failed to compile. Could not allocate memory to read log (log length = %d).\n", log_length);
                rc = -1;
                goto _done;
            }
            glGetShaderInfoLog(shader_vertex_id, log_length, NULL, log);
            log[log_length] = 0;
            l_critical("Shader failed to compile.\n%s\n", log);
            rc = -1;
            goto _done;
        }
    }

    shader_fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    {
        const char *shader_fragment_source = "#version 330 core\nvoid main() {}";
        glShaderSource(shader_fragment_id, 1, &shader_fragment_source, NULL);
        glCompileShader(shader_fragment_id);
        GLint compile_status = 0;
        glGetShaderiv(shader_fragment_id, GL_COMPILE_STATUS, &compile_status);
        if (compile_status != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(shader_fragment_id, GL_INFO_LOG_LENGTH, &log_length);
            char *log = mm_arena_alloc(&arena, log_length + 1);
            if (log == NULL) {
                l_critical("Shader failed to compile. Could not allocate memory to read log (log length = %d).\n", log_length);
                rc = -1;
                goto _done;
            }
            glGetShaderInfoLog(shader_fragment_id, log_length, NULL, log);
            log[log_length] = 0;
            l_critical("Shader failed to compile.\n%s\n", log);
            rc = -1;
            goto _done;
        }
    }

    program_id = glCreateProgram();
    {
        glAttachShader(program_id, shader_vertex_id);
        glAttachShader(program_id, shader_fragment_id);
        glLinkProgram(program_id);
        GLint link_status = 0;
        glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
        if (link_status != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
            char *log = mm_arena_alloc(&arena, log_length + 1);
            if (log == NULL) {
                l_critical("Program failed to link. Could not allocate memory to read log (log length = %d).\n", log_length);
                rc = -1;
                goto _done;
            }
            glGetShaderInfoLog(program_id, log_length, NULL, log);
            log[log_length] = 0;
            l_critical("Program failed to link.\n%s\n", log);
            rc = -1;
            goto _done;
        }
    }
    glUseProgram(program_id);

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
    if (program_id != 0) {
        glDeleteProgram(program_id);
    }
    if (shader_vertex_id != 0) {
        glDeleteShader(shader_vertex_id);
    }
    if (shader_fragment_id != 0) {
        glDeleteShader(shader_fragment_id);
    }
    mm_arena_destroy(&arena);
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    return rc;
}

