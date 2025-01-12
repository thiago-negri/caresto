#include <caresto/opengl/coo_opengl.h>
#include <engine/el_log.h>
#include <engine/eo_opengl.h>
#include <engine/et_test.h>
#include <gen/glsl_sprite_fragment.h>
#include <gen/glsl_sprite_geometry.h>
#include <gen/glsl_sprite_vertex.h>
#include <gen/glsl_tile_fragment.h>
#include <gen/glsl_tile_geometry.h>
#include <gen/glsl_tile_vertex.h>

int coo_sprite_shader_load(struct coo_sprite_shader *shader,
                          struct ea_arena *arena) {
    int rc = 0;

#ifdef SHARED
    long long timestamp =
        em_max(glsl_sprite_vertex_timestamp, glsl_sprite_geometry_timestamp,
               glsl_sprite_fragment_timestamp);
    if (timestamp <= shader->timestamp) {
        return rc;
    }
#endif

    GLuint program_id = shader->program_id;
    GLuint shader_vertex_id = 0;
    GLuint shader_geometry_id = 0;
    GLuint shader_fragment_id = 0;

    el_debug("GL: Creating sprite shader ...");

    rc = eo_shader_create(GL_VERTEX_SHADER, glsl_sprite_vertex_source, arena,
                          &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_shader_create(GL_GEOMETRY_SHADER, glsl_sprite_geometry_source,
                          arena, &shader_geometry_id);
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

    GLuint shaders[] = {shader_vertex_id, shader_geometry_id,
                        shader_fragment_id};
    rc = eo_program_link(program_id, 3, shaders, arena);
    if (rc != 0) {
        goto _err;
    }

    glDeleteShader(shader_vertex_id);
    shader_vertex_id = 0;

    glDeleteShader(shader_geometry_id);
    shader_geometry_id = 0;

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
        em_max(glsl_sprite_vertex_timestamp, glsl_sprite_geometry_timestamp,
               glsl_sprite_fragment_timestamp);
#endif
    goto _done;

_err:
    if (shader_vertex_id != 0) {
        glDeleteShader(shader_vertex_id);
    }
    if (shader_geometry_id != 0) {
        glDeleteShader(shader_geometry_id);
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
                             struct eo_texture *texture, int sprite_count,
                             struct eo_sprite_buffer *sprite_buffer) {
    // Bind GL objects
    glUseProgram(shader->program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glBindVertexArray(sprite_buffer->vertex_array_id);
    glUniformMatrix4fv(shader->g_transform_mat_id, 1, GL_FALSE,
                       (GLfloat *)transform_mat);

    // Draw sprites
    glDrawArrays(GL_POINTS, 0, sprite_count);

    // Reset GL objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

int coo_tile_shader_load(struct coo_tile_shader *shader,
                        struct ea_arena *arena) {
    int rc = 0;

#ifdef SHARED
    long long timestamp =
        em_max(glsl_tile_vertex_timestamp, glsl_tile_geometry_timestamp,
               glsl_tile_fragment_timestamp);
    if (timestamp <= shader->timestamp) {
        return rc;
    }
#endif

    el_debug("GL: Creating tile shader ...");

    GLuint program_id = shader->program_id;
    GLuint shader_vertex_id = 0;
    GLuint shader_geometry_id = 0;
    GLuint shader_fragment_id = 0;

    rc = eo_shader_create(GL_VERTEX_SHADER, glsl_tile_vertex_source, arena,
                          &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_shader_create(GL_GEOMETRY_SHADER, glsl_tile_geometry_source, arena,
                          &shader_geometry_id);
    if (rc != 0) {
        goto _err;
    }

    rc = eo_shader_create(GL_FRAGMENT_SHADER, glsl_tile_fragment_source, arena,
                          &shader_fragment_id);
    if (rc != 0) {
        goto _err;
    }

    if (program_id == 0) {
        program_id = glCreateProgram();
    }

    GLuint shaders[] = {shader_vertex_id, shader_geometry_id,
                        shader_fragment_id};
    rc = eo_program_link(program_id, 3, shaders, arena);
    if (rc != 0) {
        goto _err;
    }

    glDeleteShader(shader_vertex_id);
    shader_vertex_id = 0;

    glDeleteShader(shader_geometry_id);
    shader_geometry_id = 0;

    glDeleteShader(shader_fragment_id);
    shader_fragment_id = 0;

    GLint g_tile_size_id = glGetUniformLocation(program_id, "g_tile_size");
    if (g_tile_size_id == -1) {
        el_critical("GL: Can't find uniform 'g_tile_size'.\n");
        rc = -1;
        goto _err;
    }

    GLint g_transform_mat_id =
        glGetUniformLocation(program_id, "g_transform_mat");
    if (g_transform_mat_id == -1) {
        el_critical("GL: Can't find uniform 'g_transform_mat'.\n");
        rc = -1;
        goto _err;
    }

    shader->program_id = program_id;
    shader->g_tile_size_id = g_tile_size_id;
    shader->g_transform_mat_id = g_transform_mat_id;
#ifdef SHARED
    shader->timestamp =
        em_max(glsl_tile_vertex_timestamp, glsl_tile_geometry_timestamp,
               glsl_tile_fragment_timestamp);
#endif
    goto _done;

_err:
    if (shader_vertex_id != 0) {
        glDeleteShader(shader_vertex_id);
    }
    if (shader_geometry_id != 0) {
        glDeleteShader(shader_geometry_id);
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

void coo_tile_shader_destroy(struct coo_tile_shader *shader) {
    if (shader->program_id != 0) {
        glDeleteProgram(shader->program_id);
        shader->program_id = 0;
        shader->g_transform_mat_id = 0;
#ifdef SHARED
        shader->timestamp = 0;
#endif // SHARED
    }
}

void coo_tile_shader_render(struct coo_tile_shader *shader,
                           struct em_isize *tile_size,
                           struct em_mat4 *transform_mat,
                           struct eo_texture *texture, int tile_count,
                           struct eo_tile_buffer *tile_buffer) {
    // Bind GL objects
    glUseProgram(shader->program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glBindVertexArray(tile_buffer->vertex_array_id);
    glUniform2i(shader->g_tile_size_id, tile_size->w, tile_size->h);
    glUniformMatrix4fv(shader->g_transform_mat_id, 1, GL_FALSE,
                       (GLfloat *)transform_mat);

    // Draw tiles
    glDrawArrays(GL_POINTS, 0, tile_count);

    // Reset GL objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}