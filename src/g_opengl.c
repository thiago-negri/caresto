#include <stddef.h>

#include <g_opengl.h>
#include <l_log.h>
#include <mm_memory_management.h>

#include <gen/glsl_fragment.h>
#include <gen/glsl_vertex.h>

static const char *g_opengl_shader_type_name(GLenum type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_FRAGMENT_SHADER:
            return "fragment";
    }
    return "unknown";
}

static int g_opengl_shader_create(GLenum type, const GLchar *source, struct mm_arena *arena, GLuint *out_shader_id) {
    int rc = 0;
    GLuint shader_id = 0;

    shader_id = glCreateShader(type);
    if (shader_id == 0) {
        l_critical("Failed to create shader (type = %s).\n", g_opengl_shader_type_name(type));
        rc = -1;
        goto _err;
    }

    GLsizei count = 1;
    GLint *length = NULL; // If length is NULL, each string is assumed to be null terminated.
    glShaderSource(shader_id, count, &source, length);

    glCompileShader(shader_id);

    GLint compile_status = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        l_critical("GL: Failed to compile shader (type = %s).\n", g_opengl_shader_type_name(type));
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = mm_arena_save_offset(arena);
        char *log = mm_arena_alloc(arena, log_length);
        if (log == NULL) {
            l_critical("GL: Could not allocate memory to read log (length = %d).\n", log_length);
        } else {
            glGetShaderInfoLog(shader_id, log_length, NULL, log);
            log[log_length] = 0;
            l_critical("GL: Compile logs:\n%s\n", log);
            mm_arena_restore_offset(arena, offset);
        }
        rc = -1;
        goto _err;
    }

    *out_shader_id = shader_id;
    goto _done;

_err:
    if (shader_id != 0) {
        glDeleteShader(shader_id);
    }

_done:
    return rc;
}

int g_opengl_program_create(struct mm_arena *arena, GLuint *out_program_id) {
    int rc = 0;
    GLuint program_id = 0;
    GLuint shader_vertex_id = 0;
    GLuint shader_fragment_id = 0;

    rc = g_opengl_shader_create(GL_VERTEX_SHADER, glsl_vertex_source, arena, &shader_vertex_id);
    if (rc != 0) {
        goto _err;
    }

    rc = g_opengl_shader_create(GL_FRAGMENT_SHADER, glsl_fragment_source, arena, &shader_fragment_id);
    if (rc != 0) {
        goto _err;
    }

    program_id = glCreateProgram();

    glAttachShader(program_id, shader_vertex_id);
    glAttachShader(program_id, shader_fragment_id);

    glLinkProgram(program_id);

    GLint link_status = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        l_critical("GL: Failed to link program.\n");
        GLint log_length = 0;
        // log_length includes the null terminator
        glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        size_t offset = mm_arena_save_offset(arena);
        char *log = mm_arena_alloc(arena, log_length);
        if (log == NULL) {
            l_critical("GL: Could not allocate memory to read log (length = %d).\n", log_length);
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
    glDetachShader(program_id, shader_fragment_id);

    glDeleteShader(shader_vertex_id);
    glDeleteShader(shader_fragment_id);

    glUseProgram(program_id);

    *out_program_id = program_id;
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

