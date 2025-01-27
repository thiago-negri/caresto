#include <SDL3/SDL_surface.h>
#include <caresto/data/cdi_id.h>
#include <caresto/system/csu_ui.h>
#include <engine/et_test.h>

typedef unsigned char csu_ui_text_index;

CDI_IDS(cds_ui, csu, csu_ui_text_index, text_ids, CDS_UI_TEXTS_MAX)

ET_TEST(csu_sizes) {
    ET_ASSERT((csu_ui_text_index)CDS_UI_TEXTS_MAX == CDS_UI_TEXTS_MAX);
    ET_ASSERT((cds_ui_text_id)CDS_UI_TEXTS_MAX == CDS_UI_TEXTS_MAX);
    ET_DONE;
}

int csu_text_set(struct cds_systems *systems, cds_ui_text_id *id,
                 TTF_Font *font, const char *string, int x, int y) {
    int rc = 0;
    GLuint texture_id = 0;
    SDL_Surface *surface_argb = NULL;
    SDL_Surface *surface_rgba = NULL;

    bool is_new = *id == 0;

    int index;

    if (is_new) {
        el_assert(systems->ui.text_count < CDS_UI_TEXTS_MAX);
        index = systems->ui.text_count;
        *id = csu_text_ids_new(&systems->ui, index);
        systems->ui.text_count++;
    } else {
        index = csu_text_ids_get(&systems->ui, *id);

        // FIXME(tnegri): Check if not changed (font+string)?
        struct cds_ui_text *text = &systems->ui.texts[index];
        glDeleteTextures(1, &text->texture_id);
    }

    // Renders to ARGB surface
    surface_argb =
        TTF_RenderText_Blended(font, string, 0, (SDL_Color){255, 255, 255, 0});
    if (surface_argb == NULL) {
        const char *sdl_error = SDL_GetError();
        el_critical_fmt("SDL_ttf: Could not render text. %s\n", sdl_error);
        rc = -1;
        goto _err;
    }

    // Converts to RGBA surface
    surface_rgba = SDL_CreateSurface(surface_argb->w, surface_argb->h,
                                     SDL_PIXELFORMAT_RGBA32);
    SDL_BlitSurface(surface_argb, 0, surface_rgba, 0);

    // Send texture to GPU
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface_rgba->w, surface_rgba->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, surface_rgba->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Add to ui.texts
    struct cds_ui_text ui_text = {.x = x, .y = y, .texture_id = texture_id};
    memcpy(&systems->ui.texts[index], &ui_text, sizeof(struct cds_ui_text));

    // Add to ui.texts_gpu
    struct coo_sprite sprite = {};
    struct coo_sprite_vertex top_left = {
        .position = {.x = x, .y = y},
        .texture = {.u = 0, .v = 0},
    };
    struct coo_sprite_vertex top_right = {
        .position = {.x = x + surface_rgba->w, .y = y},
        .texture = {.u = surface_rgba->w, .v = 0},
    };
    struct coo_sprite_vertex bottom_left = {
        .position = {.x = x, .y = y + surface_rgba->h},
        .texture = {.u = 0, .v = surface_rgba->h},
    };
    struct coo_sprite_vertex bottom_right = {
        .position = {.x = x + surface_rgba->w, .y = y + surface_rgba->h},
        .texture = {.u = surface_rgba->w, .v = surface_rgba->h},
    };
    // Triangle 1
    sprite.vertex[0] = top_left;
    sprite.vertex[1] = bottom_left;
    sprite.vertex[2] = top_right;
    // Triangle 2
    sprite.vertex[3] = top_right;
    sprite.vertex[4] = bottom_left;
    sprite.vertex[5] = bottom_right;
    memcpy(&systems->ui.texts_gpu[index], &sprite, sizeof(struct coo_sprite));

    SDL_DestroySurface(surface_argb);
    SDL_DestroySurface(surface_rgba);
    goto _done;

_err:
    if (surface_argb != NULL) {
        SDL_DestroySurface(surface_argb);
    }
    if (surface_rgba != NULL) {
        SDL_DestroySurface(surface_rgba);
    }

_done:
    return rc;
}

void csu_text_clear(struct cds_systems *systems, cds_ui_text_id *id) {
    el_assert(*id != 0);

    csu_ui_text_index index = csu_text_ids_rm(&systems->ui, *id);

    struct cds_ui_text *text = &systems->ui.texts[index];
    glDeleteTextures(1, &text->texture_id);

    systems->ui.text_count--;
    if (index < systems->ui.text_count) {
        memcpy(&systems->ui.texts[index],
               &systems->ui.texts[systems->ui.text_count],
               sizeof(struct cds_ui_text));

        memcpy(&systems->ui.texts_gpu[index],
               &systems->ui.texts_gpu[systems->ui.text_count],
               sizeof(struct coo_sprite));

        csu_text_ids_move(&systems->ui, index, systems->ui.text_count);
    }

    *id = 0;
}
