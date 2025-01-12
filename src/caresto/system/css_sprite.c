#include <caresto/data/cdi_id.h>
#include <caresto/system/css_sprite.h>
#include <engine/el_log.h>
#include <engine/et_test.h>

typedef unsigned char css_sprite_index;

CDI_IDS(css_sprite_map, css, css_sprite_index, ids, CSS_SPRITES_MAX)

ET_TEST(css_sizes) {
    ET_ASSERT((css_sprite_index)CSS_SPRITES_MAX == CSS_SPRITES_MAX);
    ET_ASSERT((css_sprite_id)CSS_SPRITES_MAX == CSS_SPRITES_MAX);
    ET_DONE;
}

static void css_init_coo_sprite(struct coo_sprite *out, struct css_sprite *in) {
    int mirror_x = in->w * (in->flags & CSS_MIRROR_X);

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

void css_set(struct css_sprite_map *spritemap, css_sprite_id *id,
             struct css_sprite *sprite) {
    bool is_new = *id == 0;

    int index;

    if (is_new) {
        el_assert(spritemap->sprite_count < CSS_SPRITES_MAX);
        index = spritemap->sprite_count;
        *id = css_ids_new(spritemap, index);
        spritemap->sprite_count++;
    } else {
        index = css_ids_get(spritemap, *id);
    }

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&spritemap->sprites[index], sprite, sizeof(struct css_sprite));
    memcpy(&spritemap->sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}

void css_remove(struct css_sprite_map *spritemap, css_sprite_id *id) {
    el_assert(*id != 0);

    int index = css_ids_rm(spritemap, *id);

    spritemap->sprite_count--;
    if (index < spritemap->sprite_count) {
        memcpy(&spritemap->sprites[index],
               &spritemap->sprites[spritemap->sprite_count],
               sizeof(struct css_sprite));

        memcpy(&spritemap->sprites_gpu[index],
               &spritemap->sprites_gpu[spritemap->sprite_count],
               sizeof(struct coo_sprite));

        css_ids_move(spritemap, index, spritemap->sprite_count);
    }

    *id = 0;
}

void css_set_texture(struct css_sprite_map *spritemap, css_sprite_id id, int u,
                     int v) {
    css_sprite_index index = css_ids_get(spritemap, id);
    struct css_sprite *sprite = &spritemap->sprites[index];
    sprite->u = u;
    sprite->v = v;

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&spritemap->sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}

void css_set_position(struct css_sprite_map *spritemap, css_sprite_id id, int x,
                      int y) {
    css_sprite_index index = css_ids_get(spritemap, id);
    struct css_sprite *sprite = &spritemap->sprites[index];
    sprite->x = x;
    sprite->y = y;

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&spritemap->sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}

void css_set_flags(struct css_sprite_map *spritemap, css_sprite_id id,
                   unsigned int flags) {
    css_sprite_index index = css_ids_get(spritemap, id);
    struct css_sprite *sprite = &spritemap->sprites[index];
    sprite->flags = flags;

    struct coo_sprite coo_sprite;
    css_init_coo_sprite(&coo_sprite, sprite);

    memcpy(&spritemap->sprites_gpu[index], &coo_sprite,
           sizeof(struct coo_sprite));
}
