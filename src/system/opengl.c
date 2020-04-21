#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <stdlib.h>
#include "opengl.h"
#include "util/log.h"

int create_shader_program(
        shader_program_t *t_shader_program,
        const char *t_vert_shader_text, size_t t_vert_shader_size,
        const char *t_frag_shader_text, size_t t_frag_shader_size
) {
    GLuint vert_shader, frag_shader; // shaders

    // create vertex shader
    if (create_shader(&vert_shader, t_vert_shader_text, t_vert_shader_size, true) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "vert shader creation failed\n");

        return EXIT_FAILURE;
    }

    // create fragment shader
    if (create_shader(&frag_shader, t_frag_shader_text, t_frag_shader_size, false) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "frag shader creation failed\n");

        // prevent leaking vertex shader
        delete_shader(vert_shader);

        return EXIT_FAILURE;
    }

    // create shader program
    t_shader_program->m_shader_program = glCreateProgram();

    glAttachShader(t_shader_program->m_shader_program, vert_shader);
    glAttachShader(t_shader_program->m_shader_program, frag_shader);

    glLinkProgram(t_shader_program->m_shader_program);

    GLint success = GL_FALSE;
    glGetProgramiv(t_shader_program->m_shader_program, GL_LINK_STATUS, (int *) &success);
    if (success == GL_FALSE) {
        GLint info_log_length = 0;
        glGetProgramiv(t_shader_program->m_shader_program, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *info_log = malloc(info_log_length * sizeof(GLchar));

        if (info_log == NULL) {
            nm_log(LOG_ERROR, "could not allocate memory for log info\n");

            glDeleteProgram(t_shader_program->m_shader_program);

            delete_shader(frag_shader);
            delete_shader(vert_shader);

            return EXIT_FAILURE;
        }

        glGetProgramInfoLog(t_shader_program->m_shader_program, info_log_length, &info_log_length, info_log);
        info_log[info_log_length - 1] = '\0'; // null terminate
        nm_log(LOG_ERROR, "shader program linking failed: %s\n", info_log);
        free(info_log);

        glDeleteProgram(t_shader_program->m_shader_program);

        delete_shader(frag_shader);
        delete_shader(vert_shader);

        return EXIT_FAILURE;
    }

    // detach shaders and delete shaders
    glDetachShader(t_shader_program->m_shader_program, frag_shader);
    glDetachShader(t_shader_program->m_shader_program, vert_shader);

    delete_shader(frag_shader);
    delete_shader(vert_shader);

    return EXIT_SUCCESS;
}

int delete_shader_program(shader_program_t *t_shader_program) {
    glDeleteProgram(t_shader_program->m_shader_program);

    return EXIT_SUCCESS;
}

int create_shader(GLuint *t_shader, const char *t_shader_text, GLint t_shader_size, bool t_is_vertex) {
    if (t_is_vertex) {
        *t_shader = glCreateShader(GL_VERTEX_SHADER);
    } else {
        *t_shader = glCreateShader(GL_FRAGMENT_SHADER);
    }

    glShaderSource(*t_shader, 1, &t_shader_text, &t_shader_size);
    glCompileShader(*t_shader);
    GLint success = GL_FALSE;
    glGetShaderiv(*t_shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint info_log_length = 0; // number of characters in info log
        glGetShaderiv(*t_shader, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *info_log = malloc(info_log_length * sizeof(GLchar));

        if (info_log == NULL) {
            nm_log(LOG_ERROR, "could not allocate memory for info log\n");

            glDeleteShader(*t_shader);

            return EXIT_FAILURE;
        }

        glGetShaderInfoLog(*t_shader, info_log_length, NULL, info_log);
        info_log[info_log_length - 1] = '\0'; // null terminate
        nm_log(LOG_ERROR, "shader compilation failed: %s\n", info_log);
        free(info_log);

        glDeleteShader(*t_shader);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int delete_shader(GLuint t_shader) {
    glDeleteShader(t_shader);

    return EXIT_SUCCESS;
}

int use_shader_program(shader_program_t *t_shader_program) {
    glUseProgram(t_shader_program->m_shader_program);

    return EXIT_SUCCESS;
}

int unuse_shader_program() {
    glUseProgram(0);

    return EXIT_SUCCESS;
}

int set_mat4x4(shader_program_t *t_shader_program, const char *t_name, mat4x4 t_val) {
    GLint location = glGetUniformLocation(t_shader_program->m_shader_program, t_name);
    glUniformMatrix4fv(location, 1, GL_FALSE, t_val);

    return EXIT_SUCCESS;
}

// todo: collapse these functions
int create_tex_from_file(tex_t *t_tex, const char *t_tex_file, GLenum t_texture_unit) {
    glGenTextures(1, &t_tex->m_tex_id);
    glBindTexture(GL_TEXTURE_2D, t_tex->m_tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load and generate the texture
    int width, height, channel_count;
    unsigned char *data = stbi_load(t_tex_file, &width, &height, &channel_count, 0);
    if (data) {
        GLenum format;
        if (channel_count == 3) {
            format = GL_RGB;
        } else if (channel_count == 4) {
            format = GL_RGBA;
        } else {
            nm_log(LOG_WARN, "unknown texture format, guessing GL_RGBA\n");
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        nm_log(LOG_ERROR, "failed to load texture: %s\n", t_tex_file);
        stbi_image_free(data);

        return EXIT_FAILURE;
    }

    stbi_image_free(data);

    t_tex->m_texture_unit = t_texture_unit;

    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

int create_tex_from_mem(
        tex_t *t_tex, const char *t_tex_data, size_t t_tex_len, GLenum t_texture_unit,
        uint32_t t_channel_count
) {
    glGenTextures(1, &t_tex->m_tex_id);
    glBindTexture(GL_TEXTURE_2D, t_tex->m_tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load and generate the texture
    int width, height, channel_count;
    unsigned char *data = stbi_load_from_memory(
            (const unsigned char *) t_tex_data,
            t_tex_len, &width, &height, &channel_count, t_channel_count
    );

    if (data) {
        GLenum format;
        if (channel_count == 3) {
            format = GL_RGB;
        } else if (channel_count == 4) {
            format = GL_RGBA;
        } else {
            nm_log(LOG_WARN, "unknown texture format, guessing GL_RGBA\n");
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        nm_log(LOG_ERROR, "failed to load texture\n");
        stbi_image_free(data);

        return EXIT_FAILURE;
    }

    stbi_image_free(data);

    t_tex->m_texture_unit = t_texture_unit;

    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

int create_tex_from_buffer(
        tex_t *t_tex, const char *t_tex_data, uint32_t width, uint32_t height, GLenum t_texture_unit,
        uint32_t t_channel_count
) {
    glGenTextures(1, &t_tex->m_tex_id);
    glBindTexture(GL_TEXTURE_2D, t_tex->m_tex_id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format;
    if (t_channel_count == 3) {
        format = GL_RGB;
    } else if (t_channel_count == 4) {
        format = GL_RGBA;
    } else {
        nm_log(LOG_WARN, "unknown texture format, guessing GL_RGBA\n");
        format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, t_tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    t_tex->m_texture_unit = t_texture_unit;

    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

int bind_tex(tex_t *t_tex) {
    glActiveTexture(t_tex->m_texture_unit);
    glBindTexture(GL_TEXTURE_2D, t_tex->m_tex_id);

    return 0;
}

int unbind_tex() {
    glBindTexture(GL_TEXTURE_2D, 0);

    return EXIT_SUCCESS;
}

int delete_tex(tex_t *t_tex) {
    glDeleteTextures(1, &t_tex->m_tex_id);

    return EXIT_SUCCESS;
}

