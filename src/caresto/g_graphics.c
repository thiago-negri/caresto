#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <caresto/g_graphics.h>
#include <caresto/l_log.h>
#include <caresto/mm_memory_management.h>

#include <gen/glsl_fragment.h>
#include <gen/glsl_geometry.h>
#include <gen/glsl_vertex.h>

static const char *g_shader_type_name(GLenum type) {
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

static int g_shader_create(GLenum type, const GLchar *source,
                           struct mm_arena *arena, GLuint *out_shader_id) {
    int rc = 0;
    GLuint shader_id = 0;

    shader_id = glCreateShader(type);
    if (shader_id == 0) {
        l_critical("Failed to create shader (type = %s).\n",
                   g_shader_type_name(type));
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
                   g_shader_type_name(type));
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
    l_debug("GL: Compiled %s shader.\n", g_shader_type_name(type));

    *out_shader_id = shader_id;
    goto _done;

_err:
    if (shader_id != 0) {
        glDeleteShader(shader_id);
    }

_done:
    return rc;
}

int g_program_create(struct mm_arena *arena, struct g_program *out_program) {
    int rc = 0;
    GLuint program_id = 0;
    GLuint shader_vertex_id = 0;
    GLuint shader_geometry_id = 0;
    GLuint shader_fragment_id = 0;
    GLuint buffer_id = 0;
    GLuint vertex_array_id = 0;

    rc = g_shader_create(GL_VERTEX_SHADER, glsl_vertex_source, arena,
                         &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = g_shader_create(GL_GEOMETRY_SHADER, glsl_geometry_source, arena,
                         &shader_geometry_id);
    if (rc != 0) {
        goto _err;
    }

    rc = g_shader_create(GL_FRAGMENT_SHADER, glsl_fragment_source, arena,
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

    // Generate the VBO to be used
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, G_SPRITE_MAX * G_SPRITE_SIZE, NULL,
                 GL_DYNAMIC_DRAW);

    // Generate the VAO to be used
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, G_SPRITE_SIZE,
                          (void *)offsetof(struct g_sprite, x));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, G_SPRITE_SIZE,
                          (void *)offsetof(struct g_sprite, w));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    out_program->program_id = program_id;
    out_program->buffer_id = buffer_id;
    out_program->vertex_array_id = vertex_array_id;
    goto _done;

_err:
    if (buffer_id != 0) {
        glDeleteBuffers(1, &buffer_id);
    }
    if (vertex_array_id != 0) {
        glDeleteVertexArrays(1, &vertex_array_id);
    }
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

void g_program_destroy(struct g_program *program) {
    if (program->buffer_id != 0) {
        glDeleteBuffers(1, &program->buffer_id);
        program->buffer_id = 0;
    }
    if (program->vertex_array_id != 0) {
        glDeleteVertexArrays(1, &program->vertex_array_id);
        program->vertex_array_id = 0;
    }
    if (program->program_id != 0) {
        glDeleteProgram(program->program_id);
        program->program_id = 0;
    }
}

int g_texture_load(const char *file_path, struct g_texture *out_texture) {
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
