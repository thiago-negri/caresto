#include <caresto/opengl/coo_opengl.h>
#include <engine/el_log.h>
#include <engine/eo_opengl.h>
#include <engine/et_test.h>
#include <gen/glsl_debug_fragment.h>
#include <gen/glsl_debug_vertex.h>
#include <gen/glsl_sprite_fragment.h>
#include <gen/glsl_sprite_vertex.h>

int coo_sprite_shader_load(struct coo_sprite_shader *shader,
                           struct ea_arena *arena) {
    int rc = 0;

#ifdef SHARED
    long long timestamp =
        em_max(glsl_sprite_vertex_timestamp, glsl_sprite_fragment_timestamp);
    if (timestamp <= shader->timestamp) {
        return rc;
    }
#endif

    GLuint program_id = shader->program_id;
    GLuint shader_vertex_id = 0;
    GLuint shader_fragment_id = 0;

    el_debug("GL: Creating sprite shader ...");

    rc = eo_shader_create(GL_VERTEX_SHADER, glsl_sprite_vertex_source, arena,
                          &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_shader_create(GL_FRAGMENT_SHADER, glsl_sprite_fragment_source,
                          arena, &shader_fragment_id);
    if (rc != 0) {
        goto _err;
    }

    if (program_id == 0) {
        program_id = glCreateProgram();
    }

    GLuint shaders[] = {shader_vertex_id, shader_fragment_id};
    rc = eo_program_link(program_id, 2, shaders, arena);
    if (rc != 0) {
        goto _err;
    }

    glDeleteShader(shader_vertex_id);
    shader_vertex_id = 0;

    glDeleteShader(shader_fragment_id);
    shader_fragment_id = 0;

    GLint g_transform_mat_id =
        glGetUniformLocation(program_id, "g_transform_mat");
    if (g_transform_mat_id == -1) {
        el_critical("GL: Can't find uniform 'g_transform_mat'.\n");
        rc = -1;
        goto _err;
    }

    shader->program_id = program_id;
    shader->g_transform_mat_id = g_transform_mat_id;
#ifdef SHARED
    shader->timestamp =
        em_max(glsl_sprite_vertex_timestamp, glsl_sprite_fragment_timestamp);
#endif
    goto _done;

_err:
    if (shader_vertex_id != 0) {
        glDeleteShader(shader_vertex_id);
    }
    if (shader_fragment_id != 0) {
        glDeleteShader(shader_fragment_id);
    }
    if (program_id != 0) {
        glDeleteProgram(program_id);
    }

_done:
    return rc;
}

void coo_sprite_shader_destroy(struct coo_sprite_shader *shader) {
    if (shader->program_id != 0) {
        glDeleteProgram(shader->program_id);
        shader->program_id = 0;
        shader->g_transform_mat_id = 0;
#ifdef SHARED
        shader->timestamp = 0;
#endif // SHARED
    }
}

void coo_sprite_shader_render(struct coo_sprite_shader *shader,
                              struct em_mat4 *transform_mat,
                              struct eo_texture *texture, int vertex_count,
                              struct eo_buffer *sprite_vertex_buffer) {
    // Bind GL objects
    glUseProgram(shader->program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glBindVertexArray(sprite_vertex_buffer->vertex_array_id);
    glUniformMatrix4fv(shader->g_transform_mat_id, 1, GL_FALSE,
                       (GLfloat *)transform_mat);

    // Draw sprites
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    // Reset GL objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coo_sprite_buffer_create(struct eo_buffer *out_buffer, int count,
                              GLenum usage) {
    eo_buffer_create_start(out_buffer, count * sizeof(struct coo_sprite),
                           usage);

    glVertexAttribIPointer(
        0, 2, GL_INT, sizeof(struct coo_sprite_vertex),
        (void *)offsetof(struct coo_sprite_vertex, position.x));
    glEnableVertexAttribArray(0);

    glVertexAttribIPointer(
        1, 2, GL_INT, sizeof(struct coo_sprite_vertex),
        (void *)offsetof(struct coo_sprite_vertex, texture.u));
    glEnableVertexAttribArray(1);

    eo_buffer_create_end();
}

void coo_sprite_buffer_data(struct eo_buffer *buffer, size_t count,
                            struct coo_sprite *data) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct coo_sprite),
                    data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int coo_debug_shader_load(struct coo_debug_shader *shader,
                          struct ea_arena *arena) {
    int rc = 0;

#ifdef SHARED
    long long timestamp =
        em_max(glsl_sprite_vertex_timestamp, glsl_sprite_fragment_timestamp);
    if (timestamp <= shader->timestamp) {
        return rc;
    }
#endif

    GLuint program_id = shader->program_id;
    GLuint shader_vertex_id = 0;
    GLuint shader_fragment_id = 0;

    el_debug("GL: Creating debug shader ...");

    rc = eo_shader_create(GL_VERTEX_SHADER, glsl_debug_vertex_source, arena,
                          &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_shader_create(GL_FRAGMENT_SHADER, glsl_debug_fragment_source, arena,
                          &shader_fragment_id);
    if (rc != 0) {
        goto _err;
    }

    if (program_id == 0) {
        program_id = glCreateProgram();
    }

    GLuint shaders[] = {shader_vertex_id, shader_fragment_id};
    rc = eo_program_link(program_id, 2, shaders, arena);
    if (rc != 0) {
        goto _err;
    }

    glDeleteShader(shader_vertex_id);
    shader_vertex_id = 0;

    glDeleteShader(shader_fragment_id);
    shader_fragment_id = 0;

    GLint g_transform_mat_id =
        glGetUniformLocation(program_id, "g_transform_mat");
    if (g_transform_mat_id == -1) {
        el_critical("GL: Can't find uniform 'g_transform_mat'.\n");
        rc = -1;
        goto _err;
    }

    shader->program_id = program_id;
    shader->g_transform_mat_id = g_transform_mat_id;
#ifdef SHARED
    shader->timestamp =
        em_max(glsl_sprite_vertex_timestamp, glsl_sprite_fragment_timestamp);
#endif
    goto _done;

_err:
    if (shader_vertex_id != 0) {
        glDeleteShader(shader_vertex_id);
    }
    if (shader_fragment_id != 0) {
        glDeleteShader(shader_fragment_id);
    }
    if (program_id != 0) {
        glDeleteProgram(program_id);
    }

_done:
    return rc;
}

void coo_debug_shader_destroy(struct coo_debug_shader *shader) {
    if (shader->program_id != 0) {
        glDeleteProgram(shader->program_id);
        shader->program_id = 0;
        shader->g_transform_mat_id = 0;
#ifdef SHARED
        shader->timestamp = 0;
#endif // SHARED
    }
}

void coo_debug_buffer_create(struct eo_buffer *out_buffer, int count) {
    eo_buffer_create_start(out_buffer, count * sizeof(struct coo_debug),
                           GL_STREAM_DRAW);

    glVertexAttribIPointer(
        0, 2, GL_INT, sizeof(struct coo_debug_vertex),
        (void *)offsetof(struct coo_debug_vertex, position.x));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(struct coo_debug_vertex),
                          (void *)offsetof(struct coo_debug_vertex, color.r));
    glEnableVertexAttribArray(1);

    eo_buffer_create_end();
}

void coo_debug_buffer_data(struct eo_buffer *buffer, size_t count,
                           struct coo_debug *data) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct coo_debug), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void coo_debug_shader_render(struct coo_debug_shader *shader,
                             struct em_mat4 *transform_mat, int vertex_count,
                             struct eo_buffer *debug_vertex_buffer) {
    // Bind GL objects
    glUseProgram(shader->program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(debug_vertex_buffer->vertex_array_id);
    glUniformMatrix4fv(shader->g_transform_mat_id, 1, GL_FALSE,
                       (GLfloat *)transform_mat);

    // Draw sprites
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    // Reset GL objects
    glBindVertexArray(0);
    glUseProgram(0);
}
