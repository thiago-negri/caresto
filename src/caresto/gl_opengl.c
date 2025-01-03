#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <caresto/gl_opengl.h>
#include <caresto/l_log.h>
#include <caresto/mm_memory_management.h>
#include <caresto/t_test.h>

#include <gen/glsl_fragment.h>
#include <gen/glsl_geometry.h>
#include <gen/glsl_vertex.h>

void gl_identity(struct gl_mat4 *out) {
    out->ax = 1.0f;
    out->ay = 0.0f;
    out->az = 0.0f;
    out->aw = 0.0f;
    out->bx = 0.0f;
    out->by = 1.0f;
    out->bz = 0.0f;
    out->bw = 0.0f;
    out->cx = 0.0f;
    out->cy = 0.0f;
    out->cz = 1.0f;
    out->cw = 0.0f;
    out->dx = 0.0f;
    out->dy = 0.0f;
    out->dz = 0.0f;
    out->dw = 1.0f;
}

void gl_ortho(struct gl_mat4 *out, GLfloat left, GLfloat right, GLfloat top,
              GLfloat bottom, GLfloat near, GLfloat far) {
    out->ax = 2.0f / (right - left);
    out->ay = 0.0f;
    out->az = 0.0f;
    out->aw = -1.0f * (right + left) / (right - left);
    out->bx = 0.0f;
    out->by = -2.0f / (top - bottom);
    out->bz = 0.0f;
    out->bw = -1.0f * (top + bottom) / (top - bottom);
    out->cx = 0.0f;
    out->cy = 0.0f;
    out->cz = -2.0f / (far - near);
    out->cw = -1.0f * (far + near) / (far - near);
    out->dx = 0.0f;
    out->dy = 0.0f;
    out->dz = 0.0f;
    out->dw = 1.0f;
}

T_TEST(ortho) {
    struct gl_mat4 a = {.values = {0.0f}};
    gl_ortho(&a, 0.0f, 360.0f, 0.0f, 640.0f, 0.0f, 1.0f);
    T_ASSERT(a.ax >= 0.005555f && a.ax <= 0.005557f);
    T_ASSERT(a.ay == 0.0f);
    T_ASSERT(a.az == 0.0f);
    T_ASSERT(a.aw == -1.0f);
    T_ASSERT(a.bx == 0.0f);
    T_ASSERT(a.by >= 0.003124f && a.by <= 0.003126f);
    T_ASSERT(a.bz == 0.0f);
    T_ASSERT(a.bw == 1.0f);
    T_ASSERT(a.cx == 0.0f);
    T_ASSERT(a.cy == 0.0f);
    T_ASSERT(a.cz == -2.0f);
    T_ASSERT(a.cw == -1.0f);
    T_ASSERT(a.dx == 0.0f);
    T_ASSERT(a.dy == 0.0f);
    T_ASSERT(a.dz == 0.0f);
    T_ASSERT(a.dw == 1.0f);
    T_DONE;
}

static const char *gl_shader_type_name(GLenum type) {
    switch (type) {
    case GL_VERTEX_SHADER:
        return "vertex";
    case GL_FRAGMENT_SHADER:
        return "fragment";
    case GL_GEOMETRY_SHADER:
        return "geometry";
    }
    return "unknown";
}

static int gl_shader_create(GLenum type, const GLchar *source,
                            struct mm_arena *arena, GLuint *out_shader_id) {
    int rc = 0;
    GLuint shader_id = 0;

    shader_id = glCreateShader(type);
    if (shader_id == 0) {
        l_critical("Failed to create shader (type = %s).\n",
                   gl_shader_type_name(type));
        rc = -1;
        goto _err;
    }

    GLsizei count = 1;
    GLint *length = NULL; // If length is NULL, each string is assumed to be
                          // null terminated.
    glShaderSource(shader_id, count, &source, length);

    glCompileShader(shader_id);

    GLint compile_status = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        l_critical("GL: Failed to compile shader (type = %s).\n",
                   gl_shader_type_name(type));
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = mm_arena_save_offset(arena);
        char *log = mm_arena_alloc(arena, log_length);
        if (log == NULL) {
            l_critical(
                "GL: Could not allocate memory to read log (length = %d).\n",
                log_length);
        } else {
            glGetShaderInfoLog(shader_id, log_length, NULL, log);
            log[log_length] = 0;
            l_critical("GL: Compile logs:\n%s\n", log);
            mm_arena_restore_offset(arena, offset);
        }
        rc = -1;
        goto _err;
    }
    l_debug("GL: Compiled %s shader.\n", gl_shader_type_name(type));

    *out_shader_id = shader_id;
    goto _done;

_err:
    if (shader_id != 0) {
        glDeleteShader(shader_id);
    }

_done:
    return rc;
}

int gl_program_create(struct mm_arena *arena, struct gl_program *out_program) {
    int rc = 0;
    GLuint program_id = 0;
    GLuint shader_vertex_id = 0;
    GLuint shader_geometry_id = 0;
    GLuint shader_fragment_id = 0;

    rc = gl_shader_create(GL_VERTEX_SHADER, glsl_vertex_source, arena,
                          &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = gl_shader_create(GL_GEOMETRY_SHADER, glsl_geometry_source, arena,
                          &shader_geometry_id);
    if (rc != 0) {
        goto _err;
    }

    rc = gl_shader_create(GL_FRAGMENT_SHADER, glsl_fragment_source, arena,
                          &shader_fragment_id);
    if (rc != 0) {
        goto _err;
    }

    program_id = glCreateProgram();

    glAttachShader(program_id, shader_vertex_id);
    glAttachShader(program_id, shader_geometry_id);
    glAttachShader(program_id, shader_fragment_id);

    glLinkProgram(program_id);

    GLint link_status = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        l_critical("GL: Failed to link program.\n");
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = mm_arena_save_offset(arena);
        char *log = mm_arena_alloc(arena, log_length);
        if (log == NULL) {
            l_critical(
                "GL: Could not allocate memory to read log (length = %d).\n",
                log_length);
        } else {
            glGetProgramInfoLog(program_id, log_length, NULL, log);
            log[log_length] = 0;
            l_critical("GL: Link logs:\n%s\n", log);
            mm_arena_restore_offset(arena, offset);
        }
        rc = -1;
        goto _err;
    }

    glDetachShader(program_id, shader_vertex_id);
    glDetachShader(program_id, shader_geometry_id);
    glDetachShader(program_id, shader_fragment_id);

    glDeleteShader(shader_vertex_id);
    shader_vertex_id = 0;

    glDeleteShader(shader_geometry_id);
    shader_geometry_id = 0;

    glDeleteShader(shader_fragment_id);
    shader_fragment_id = 0;

    GLint g_transform_mat_id =
        glGetUniformLocation(program_id, "g_transform_mat");
    if (g_transform_mat_id == -1) {
        l_critical("GL: Can't find uniform 'g_transform_mat'.\n");
        rc = -1;
        goto _err;
    }

    out_program->program_id = program_id;
    out_program->g_transform_mat_id = g_transform_mat_id;
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

void gl_program_destroy(struct gl_program *program) {
    if (program->program_id != 0) {
        glDeleteProgram(program->program_id);
        program->program_id = 0;
    }
}

void gl_program_render(struct gl_program *program,
                       struct gl_mat4 *g_transform_mat,
                       struct gl_texture *texture, size_t sprite_count,
                       struct gl_sprite_buffer *sprite_buffer) {
    // Bind GL objects
    glUseProgram(program->program_id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glBindVertexArray(sprite_buffer->vertex_array_id);
    glUniformMatrix4fv(program->g_transform_mat_id, 1, GL_FALSE,
                       g_transform_mat->values);

    // Draw sprites
    glDrawArrays(GL_POINTS, 0, sprite_count);

    // Reset GL objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void gl_sprite_buffer_create(GLsizei count,
                             struct gl_sprite_buffer *out_sprite_buffer) {
    GLuint buffer_id = 0;
    GLuint vertex_array_id = 0;

    // Generate the VBO to be used
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(struct gl_sprite), NULL,
                 GL_DYNAMIC_DRAW);

    // Generate the VAO to be used
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct gl_sprite),
                          (void *)offsetof(struct gl_sprite, x));
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_INT, sizeof(struct gl_sprite),
                           (void *)offsetof(struct gl_sprite, w));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(2, 2, GL_INT, sizeof(struct gl_sprite),
                           (void *)offsetof(struct gl_sprite, u));
    glEnableVertexAttribArray(2);

    // Reset GL objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    out_sprite_buffer->buffer_id = buffer_id;
    out_sprite_buffer->vertex_array_id = vertex_array_id;
}

void gl_sprite_buffer_destroy(struct gl_sprite_buffer *sprite_buffer) {
    if (sprite_buffer->vertex_array_id != 0) {
        glDeleteVertexArrays(1, &sprite_buffer->vertex_array_id);
        sprite_buffer->vertex_array_id = 0;
    }
    if (sprite_buffer->buffer_id != 0) {
        glDeleteBuffers(1, &sprite_buffer->buffer_id);
        sprite_buffer->buffer_id = 0;
    }
}

void gl_sprite_buffer_data(struct gl_sprite_buffer *buffer, size_t count,
                           struct gl_sprite *data) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct gl_sprite), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int gl_texture_load(const char *file_path, struct gl_texture *out_texture) {
    int rc = 0;
    GLuint texture_id = 0;
    unsigned char *image_data = NULL;

    int width = 0;
    int height = 0;
    int channels = 0;
    image_data = stbi_load(file_path, &width, &height, &channels, 0);
    if (!image_data) {
        l_critical("GL: Can't load image %s.\n", file_path);
        rc = -1;
        goto _err;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image_data);

    out_texture->id = texture_id;
    goto _done;

_err:
    if (image_data != NULL) {
        stbi_image_free(image_data);
    }
    if (texture_id != 0) {
        glDeleteTextures(1, &texture_id);
    }

_done:
    return rc;
}

void gl_texture_destroy(struct gl_texture *texture) {
    if (texture->id != 0) {
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
    }
}

void gl_debug_message_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *user_param) {
    l_debug("GL: Callback: %d %d %d %d %s\n", source, type, id, severity,
            message);
}
