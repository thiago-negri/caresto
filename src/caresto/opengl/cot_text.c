#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <caresto/opengl/cot_text.h>
#include <engine/el_log.h>

void render_hello_world_texture(GLuint texture_id) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(100, 0);
    glTexCoord2f(1, 1);
    glVertex2f(100, 100);
    glTexCoord2f(0, 1);
    glVertex2f(0, 100);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

int load_hello_world_texture(GLuint *out_texture_id) {
#ifdef SHARED
    // Apparently this needs to be initialized within the shared library
    // For release builds, main initializes it
    TTF_Init();
#endif

    int rc = 0;
    GLuint texture_id = 0;
    TTF_Font *font = NULL;
    SDL_Surface *surface_argb = NULL;
    SDL_Surface *surface_rgba = NULL;

    // Load font
    font = TTF_OpenFont("assets/Montserrat-Regular.ttf", 24.0f);
    if (font == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical_fmt("SDL_ttf: Could not load Montserrat-Regular. %s\n",
                        sdl_error);
        rc = -1;
        goto _err;
    }

    // Renders to ARGB surface
    surface_argb = TTF_RenderText_Blended(font, "utilitários", 0,
                                          (SDL_Color){255, 0, 0, 0});
    if (surface_argb == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical_fmt("SDL_ttf: Could not render text. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Converta to RGBA surface
    surface_rgba = SDL_CreateSurface(surface_argb->w, surface_argb->h,
                                     SDL_PIXELFORMAT_RGBA32);
    SDL_BlitSurface(surface_argb, 0, surface_rgba, 0);

    // Send to GPU
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface_rgba->w, surface_rgba->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, surface_rgba->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    el_debug("TTF: Text rendered.\n");
    *out_texture_id = texture_id;

    TTF_CloseFont(font);
    SDL_DestroySurface(surface_argb);
    SDL_DestroySurface(surface_rgba);
    goto _done;

_err:
    if (font != NULL) {
        TTF_CloseFont(font);
    }
    if (surface_argb != NULL) {
        SDL_DestroySurface(surface_argb);
    }
    if (surface_rgba != NULL) {
        SDL_DestroySurface(surface_rgba);
    }
    if (texture_id != 0) {
        glDeleteTextures(1, &texture_id);
    }

_done:
    return rc;
}
