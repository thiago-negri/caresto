#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <caresto/data/cds_systems.h>

int csu_text_set(struct cds_systems *systems, cds_ui_text_id *id,
                 TTF_Font *font, const char *string, int x, int y);

void csu_text_clear(struct cds_systems *systems, cds_ui_text_id *id);
