#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <engine/egl_opengl.h>
#include <engine/el_log.h>
#include <engine/em_memory.h>
#include <engine/et_test.h>
#include <engine/eu_utils.h>

void egl_identity(struct egl_mat4 *out) {
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

void egl_ortho(struct egl_mat4 *out, GLfloat left, GLfloat right, GLfloat top,
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

ET_TEST(ortho) {
    struct egl_mat4 a = {.values = {0.0f}};
    egl_ortho(&a, 0.0f, 360.0f, 0.0f, 640.0f, 0.0f, 1.0f);
    ET_ASSERT(a.ax >= 0.005555f && a.ax <= 0.005557f);
    ET_ASSERT(a.ay == 0.0f);
    ET_ASSERT(a.az == 0.0f);
    ET_ASSERT(a.aw == -1.0f);
    ET_ASSERT(a.bx == 0.0f);
    ET_ASSERT(a.by >= 0.003124f && a.by <= 0.003126f);
    ET_ASSERT(a.bz == 0.0f);
    ET_ASSERT(a.bw == 1.0f);
    ET_ASSERT(a.cx == 0.0f);
    ET_ASSERT(a.cy == 0.0f);
    ET_ASSERT(a.cz == -2.0f);
    ET_ASSERT(a.cw == -1.0f);
    ET_ASSERT(a.dx == 0.0f);
    ET_ASSERT(a.dy == 0.0f);
    ET_ASSERT(a.dz == 0.0f);
    ET_ASSERT(a.dw == 1.0f);
    ET_DONE;
}

static const char *egl_shader_type_name(GLenum type) {
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

int egl_shader_create(GLenum type, const GLchar *source, struct em_arena *arena,
                      GLuint *out_shader_id) {
    int rc = 0;
    GLuint shader_id = 0;

    shader_id = glCreateShader(type);
    if (shader_id == 0) {
        el_critical("Failed to create shader (type = %s).\n",
                    egl_shader_type_name(type));
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
        el_critical("GL: Failed to compile shader (type = %s).\n",
                    egl_shader_type_name(type));
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = em_arena_save_offset(arena);
        char *log = em_arena_alloc(arena, log_length);
        if (log == NULL) {
            el_critical(
                "GL: Could not allocate memory to read log (length = %d).\n",
                log_length);
        } else {
            glGetShaderInfoLog(shader_id, log_length, NULL, log);
            log[log_length] = 0;
            el_critical("GL: Compile logs:\n%s\n", log);
            em_arena_restore_offset(arena, offset);
        }
        rc = -1;
        goto _err;
    }
    el_debug("GL: Compiled %s shader.\n", egl_shader_type_name(type));

    *out_shader_id = shader_id;
    goto _done;

_err:
    if (shader_id != 0) {
        glDeleteShader(shader_id);
    }

_done:
    return rc;
}

int egl_program_link(GLuint program_id, size_t shader_count, GLuint *shaders,
                     struct em_arena *arena) {
    int rc = 0;

    for (size_t i = 0; i < shader_count; i++) {
        glAttachShader(program_id, shaders[i]);
    }

    glLinkProgram(program_id);

    GLint link_status = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        el_critical("GL: Failed to link program.\n");
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = em_arena_save_offset(arena);
        char *log = em_arena_alloc(arena, log_length);
        if (log == NULL) {
            el_critical(
                "GL: Could not allocate memory to read log (length = %d).\n",
                log_length);
        } else {
            glGetProgramInfoLog(program_id, log_length, NULL, log);
            log[log_length] = 0;
            el_critical("GL: Link logs:\n%s\n", log);
            em_arena_restore_offset(arena, offset);
        }
        rc = -1;
    }

    for (size_t i = 0; i < shader_count; i++) {
        glDetachShader(program_id, shaders[i]);
    }

    return rc;
}

void egl_sprite_buffer_create(GLsizei count,
                              struct egl_sprite_buffer *out_sprite_buffer) {
    GLuint buffer_id = 0;
    GLuint vertex_array_id = 0;

    // Generate the VBO to be used
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(struct egl_sprite), NULL,
                 GL_DYNAMIC_DRAW);

    // Generate the VAO to be used
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct egl_sprite),
                          (void *)offsetof(struct egl_sprite, x));
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_INT, sizeof(struct egl_sprite),
                           (void *)offsetof(struct egl_sprite, w));
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(2, 2, GL_INT, sizeof(struct egl_sprite),
                           (void *)offsetof(struct egl_sprite, u));
    glEnableVertexAttribArray(2);

    // Reset GL objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    out_sprite_buffer->buffer_id = buffer_id;
    out_sprite_buffer->vertex_array_id = vertex_array_id;
}

void egl_sprite_buffer_destroy(struct egl_sprite_buffer *sprite_buffer) {
    if (sprite_buffer->vertex_array_id != 0) {
        glDeleteVertexArrays(1, &sprite_buffer->vertex_array_id);
        sprite_buffer->vertex_array_id = 0;
    }
    if (sprite_buffer->buffer_id != 0) {
        glDeleteBuffers(1, &sprite_buffer->buffer_id);
        sprite_buffer->buffer_id = 0;
    }
}

void egl_sprite_buffer_data(struct egl_sprite_buffer *buffer, size_t count,
                            struct egl_sprite *data) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct egl_sprite),
                    data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int egl_texture_load(const char *file_path, struct egl_texture *out_texture) {
    int rc = 0;
    GLuint texture_id = 0;
    unsigned char *image_data = NULL;

#ifdef SHARED
    long long timestamp = eu_file_timestamp(file_path);
    if (out_texture->id != 0 && timestamp <= out_texture->timestamp) {
        goto _done;
    }
#endif

    int width = 0;
    int height = 0;
    int channels = 0;
    image_data = stbi_load(file_path, &width, &height, &channels, 0);
    if (!image_data) {
        el_critical("GL: Can't load image %s.\n", file_path);
        rc = -1;
        goto _err;
    }

    if (out_texture->id == 0) {
        glGenTextures(1, &texture_id);
    } else {
        texture_id = out_texture->id;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image_data);

    el_debug("GL: Texture loaded: %s.\n", file_path);
    out_texture->id = texture_id;
#if SHARED
    out_texture->timestamp = timestamp;
#endif
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

void egl_texture_destroy(struct egl_texture *texture) {
    if (texture->id != 0) {
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
    }
}

void egl_debug_message_callback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *user_param) {
    el_debug("GL: Callback: %d %d %d %d %s\n", source, type, id, severity,
             message);
}
