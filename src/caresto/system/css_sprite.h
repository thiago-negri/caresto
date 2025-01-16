#pragma once

#include <caresto/data/cds_systems.h>

void css_set(struct cds_systems *systems, cds_sprite_id *id,
             struct cds_sprite *sprite);

void css_set_texture(struct cds_systems *systems, cds_sprite_id id, int u,
                     int v);

void css_set_position(struct cds_systems *systems, cds_sprite_id id, int x,
                      int y);

void css_set_flags(struct cds_systems *systems, cds_sprite_id id,
                   unsigned int flags);

void css_remove(struct cds_systems *systems, cds_sprite_id *id);
