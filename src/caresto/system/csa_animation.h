#pragma once

#include <caresto/data/cds_systems.h>

void csa_play(struct cds_systems *systems, cds_animation_id *id,
              enum gen_animation_index animation_index,
              cds_sprite_id sprite_id);

void csa_done(struct cds_systems *systems, cds_animation_id *id);

void csa_frame(struct cds_systems *systems, double delta_time);
