#include <caresto/data/cdi_id.h>
#include <caresto/system/css_sprite.h>
#include <engine/el_log.h>
#include <engine/et_test.h>

typedef unsigned char css_sprite_index;

CDI_IDS(cds_sprite_map, css, css_sprite_index, ids, CDS_SPRITES_MAX)

ET_TEST(css_sizes) {
    ET_ASSERT((css_sprite_index)CDS_SPRITES_MAX == CDS_SPRITES_MAX);
    ET_ASSERT((cds_sprite_id)CDS_SPRITES_MAX == CDS_SPRITES_MAX);
    ET_DONE;
}

static void css_init_coo_sprite(struct coo_sprite *out, struct cds_sprite *in) {
    int mirror_x = in->w * (in->flags & CDS_SPRITE_MIRROR_X);

    struct coo_sprite_vertex top_left = {
        .position = {.x = in->x, .y = in->y},
        .texture = {.u = in->u + mirror_x, .v = in->v},
    };

    struct coo_sprite_vertex top_right = {
        .position = {.x = in->x + in->w, .y = in->y},
        .texture = {.u = in->u + in->w - mirror_x, .v = in->v},
    };

    struct coo_sprite_vertex bottom_left = {
        .position = {.x = in->x, .y = in->y + in->h},
        .texture = {.u = in->u + mirror_x, .v = in->v + in->h},
    };

    struct coo_sprite_vertex bottom_right = {
        .position = {.x = in->x + in->w, .y = in->y + in->h},
        .texture = {.u = in->u + in->w - mirror_x, .v = in->v + in->h},
    };

    // Triangle 1
    out->vertex[0] = top_left;
    out->vertex[1] = bottom_left;
    out->vertex[2] = top_right;

    // Triangle 2
    out->vertex[3] = top_right;
    out->vertex[4] = bottom_left;
    out->vertex[5] = bottom_right;
}

void css_set(struct cds_systems *systems, cds_sprite_id *id,
             struct cds_sprite *sprite) {
    bool is_new = *id == 0;

    int index;

    if (is_new) {
        el_assert(systems->sprite_map.sprite_count < CDS_SPRITES_MAX);
        index = systems->sprite_map.sprite_count;
        *id = css_ids_new(&systems->sprite_map, index);
        systems->sprite_map.sprite_count++;
    } else {
        index = css_ids_get(&systems->sprite_map, *id);
    }

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&systems->sprite_map.sprites[index], sprite,
           sizeof(struct cds_sprite));
    memcpy(&systems->sprite_map.sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}

void css_remove(struct cds_systems *systems, cds_sprite_id *id) {
    el_assert(*id != 0);

    int index = css_ids_rm(&systems->sprite_map, *id);

    systems->sprite_map.sprite_count--;
    if (index < systems->sprite_map.sprite_count) {
        memcpy(&systems->sprite_map.sprites[index],
               &systems->sprite_map.sprites[systems->sprite_map.sprite_count],
               sizeof(struct cds_sprite));

        memcpy(
            &systems->sprite_map.sprites_gpu[index],
            &systems->sprite_map.sprites_gpu[systems->sprite_map.sprite_count],
            sizeof(struct coo_sprite));

        css_ids_move(&systems->sprite_map, index,
                     systems->sprite_map.sprite_count);
    }

    *id = 0;
}

void css_set_texture(struct cds_systems *systems, cds_sprite_id id, int u,
                     int v) {
    css_sprite_index index = css_ids_get(&systems->sprite_map, id);
    struct cds_sprite *sprite = &systems->sprite_map.sprites[index];
    sprite->u = u;
    sprite->v = v;

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&systems->sprite_map.sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}

void css_set_position(struct cds_systems *systems, cds_sprite_id id, int x,
                      int y) {
    css_sprite_index index = css_ids_get(&systems->sprite_map, id);
    struct cds_sprite *sprite = &systems->sprite_map.sprites[index];
    sprite->x = x;
    sprite->y = y;

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&systems->sprite_map.sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}

void css_set_flags(struct cds_systems *systems, cds_sprite_id id,
                   unsigned int flags) {
    css_sprite_index index = css_ids_get(&systems->sprite_map, id);
    struct cds_sprite *sprite = &systems->sprite_map.sprites[index];
    sprite->flags = flags;

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&systems->sprite_map.sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}
