#ifndef G_GAME_H
#define G_GAME_H

#include <stdint.h>
#include <stdbool.h>

#include <caresto/gl_opengl.h>
#include <caresto/mm_memory_management.h>

struct g_frame {
    uint64_t delta_time;

    // These should probably be somewhere else?
    struct gl_program *program;
    struct gl_mat4 *camera_transform;
    struct gl_texture *sprite_atlas;
    struct gl_sprite_buffer *sprite_buffer;
};

void *g_init(struct mm_arena *persistent_storage);

bool g_process_frame(struct g_frame *frame, void *data);

#endif // G_GAME_H
