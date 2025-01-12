#define STB_IMAGE_IMPLEMENTATION

#include <engine/ea_allocator.h>
#include <engine/ef_file.h>
#include <engine/el_log.h>
#include <engine/eo_opengl.h>
#include <engine/et_test.h>
#include <stb_image.h>
#include <stddef.h>

static const char *eo_shader_type_name(GLenum type) {
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

int eo_shader_create(GLenum type, const GLchar *source, struct ea_arena *arena,
                     GLuint *out_shader_id) {
    int rc = 0;
    GLuint shader_id = 0;

    shader_id = glCreateShader(type);
    if (shader_id == 0) {
        el_critical_fmt("Failed to create shader (type = %s).\n",
                        eo_shader_type_name(type));
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
        el_critical_fmt("GL: Failed to compile shader (type = %s).\n",
                        eo_shader_type_name(type));
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = ea_arena_save_offset(arena);
        char *log = ea_arena_alloc(arena, log_length);
        if (log == NULL) {
            el_critical_fmt(
                "GL: Could not allocate memory to read log (length = %d).\n",
                log_length);
        } else {
            glGetShaderInfoLog(shader_id, log_length, NULL, log);
            log[log_length] = 0;
            el_critical_fmt("GL: Compile logs:\n%s\n", log);
            ea_arena_restore_offset(arena, offset);
        }
        rc = -1;
        goto _err;
    }
    el_debug_fmt("GL: Compiled %s shader.\n", eo_shader_type_name(type));

    *out_shader_id = shader_id;
    goto _done;

_err:
    if (shader_id != 0) {
        glDeleteShader(shader_id);
    }

_done:
    return rc;
}

int eo_program_link(GLuint program_id, size_t shader_count, GLuint *shaders,
                    struct ea_arena *arena) {
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
        size_t offset = ea_arena_save_offset(arena);
        char *log = ea_arena_alloc(arena, log_length);
        if (log == NULL) {
            el_critical_fmt(
                "GL: Could not allocate memory to read log (length = %d).\n",
                log_length);
        } else {
            glGetProgramInfoLog(program_id, log_length, NULL, log);
            log[log_length] = 0;
            el_critical_fmt("GL: Link logs:\n%s\n", log);
            ea_arena_restore_offset(arena, offset);
        }
        rc = -1;
        goto _err;
    }

    for (size_t i = 0; i < shader_count; i++) {
        glDetachShader(program_id, shaders[i]);
    }

    el_debug("GL: Linked program.\n");

_err:
    return rc;
}

void eo_sprite_buffer_create(GLsizei count,
                             struct eo_sprite_buffer *out_sprite_buffer) {
    GLuint buffer_id = 0;
    GLuint vertex_array_id = 0;

    // Generate the VBO to be used
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(struct eo_sprite), NULL,
                 GL_DYNAMIC_DRAW);

    // Generate the VAO to be used
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct eo_sprite),
                          (void *)offsetof(struct eo_sprite, position.x));
    glEnableVertexAttribArray(0);

    glVertexAttribIPointer(1, 2, GL_INT, sizeof(struct eo_sprite),
                           (void *)offsetof(struct eo_sprite, size.w));
    glEnableVertexAttribArray(1);

    glVertexAttribIPointer(
        2, 2, GL_INT, sizeof(struct eo_sprite),
        (void *)offsetof(struct eo_sprite, texture_offset.u));
    glEnableVertexAttribArray(2);

    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(struct eo_sprite),
                           (void *)offsetof(struct eo_sprite, flags));
    glEnableVertexAttribArray(3);

    // Reset GL objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    out_sprite_buffer->buffer_id = buffer_id;
    out_sprite_buffer->vertex_array_id = vertex_array_id;
}

void eo_sprite_buffer_destroy(struct eo_sprite_buffer *sprite_buffer) {
    if (sprite_buffer->vertex_array_id != 0) {
        glDeleteVertexArrays(1, &sprite_buffer->vertex_array_id);
        sprite_buffer->vertex_array_id = 0;
    }
    if (sprite_buffer->buffer_id != 0) {
        glDeleteBuffers(1, &sprite_buffer->buffer_id);
        sprite_buffer->buffer_id = 0;
    }
}

void eo_sprite_buffer_data(struct eo_sprite_buffer *buffer, size_t count,
                           struct eo_sprite *data) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct eo_sprite), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void eo_tile_buffer_create(GLsizei count,
                           struct eo_tile_buffer *out_tile_buffer) {
    GLuint buffer_id = 0;
    GLuint vertex_array_id = 0;

    // Generate the VBO to be used
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(struct eo_tile), NULL,
                 GL_DYNAMIC_DRAW);

    // Generate the VAO to be used
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct eo_tile),
                          (void *)offsetof(struct eo_tile, x));
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 2, GL_INT, sizeof(struct eo_tile),
                           (void *)offsetof(struct eo_tile, u));
    glEnableVertexAttribArray(1);

    // Reset GL objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    out_tile_buffer->buffer_id = buffer_id;
    out_tile_buffer->vertex_array_id = vertex_array_id;
}

void eo_tile_buffer_destroy(struct eo_tile_buffer *tile_buffer) {
    if (tile_buffer->vertex_array_id != 0) {
        glDeleteVertexArrays(1, &tile_buffer->vertex_array_id);
        tile_buffer->vertex_array_id = 0;
    }
    if (tile_buffer->buffer_id != 0) {
        glDeleteBuffers(1, &tile_buffer->buffer_id);
        tile_buffer->buffer_id = 0;
    }
}

void eo_tile_buffer_data(struct eo_tile_buffer *buffer, size_t count,
                         struct eo_tile *data) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(struct eo_tile), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int eo_texture_load(const char *file_path, struct eo_texture *out_texture) {
    int rc = 0;
    GLuint texture_id = 0;
    unsigned char *image_data = NULL;

#ifdef SHARED
    long long timestamp = ef_timestamp(file_path);
    if (out_texture->id != 0 && timestamp <= out_texture->timestamp) {
        goto _done;
    }
#endif

    int width = 0;
    int height = 0;
    int channels = 0;
    image_data = stbi_load(file_path, &width, &height, &channels, 0);
    if (!image_data) {
        el_critical_fmt("GL: Can't load image %s.\n", file_path);
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

    el_debug_fmt("GL: Texture loaded: %s.\n", file_path);
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

void eo_texture_destroy(struct eo_texture *texture) {
    if (texture->id != 0) {
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
    }
}

void eo_debug_message_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei /*length*/,
                               const GLchar *message,
                               const void * /*user_param*/) {
    el_debug_fmt("GL: Callback: %d %d %d %d %s\n", source, type, id, severity,
                 message);
}
