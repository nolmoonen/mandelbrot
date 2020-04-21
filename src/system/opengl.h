#ifndef NM_OPENGL_H
#define NM_OPENGL_H

#include <glad/glad.h>
#include <stdbool.h>
#include <linmath.h>

typedef struct {
    GLuint m_shader_program;
} shader_program_t;

/**
 * Returns {@code EXIT_SUCCESS} on success, {@code EXIT_FAILURE} otherwise.
 * If {@code EXIT_SUCCESS} is returned, a call to {@code delete_shader} is required before the executable terminates.
 */
int create_shader_program(
        shader_program_t *t_shader_program,
        const char *t_vert_shader_text, size_t t_vert_shader_size,
        const char *t_frag_shader_text, size_t t_frag_shader_size
);

int delete_shader_program(shader_program_t *t_shader_program);

/**
 * Returns {@code EXIT_SUCCESS} on success, {@code EXIT_FAILURE} otherwise.
 * If {@code EXIT_SUCCESS} is returned, a call to {@code delete_shader} is required before the executable terminates.
 * If {@param t_is_vertex} is {@code true}, create vertex shader. Otherwise, create fragment shader.
 */
int create_shader(GLuint *t_shader, const char *t_shader_text, GLint t_shader_size, bool t_is_vertex);

int delete_shader(GLuint t_shader);

int use_shader_program(shader_program_t *t_shader_program);

int unuse_shader_program();

int set_mat4x4(shader_program_t *t_shader_program, const char *t_name, mat4x4 t_val);

typedef struct {
    /** Id of the texture. */
    GLuint m_tex_id;
    /** Texture unit. */
    GLenum m_texture_unit;
} tex_t;

/** Png file. */
int create_tex_from_file(tex_t *t_tex, const char *t_tex_file, GLenum t_texture_unit);

/** Png file in memory. */
int create_tex_from_mem(
        tex_t *t_tex, const char *t_tex_data, size_t t_tex_len, GLenum t_texture_unit,
        uint32_t t_channel_count
);

int create_tex_from_buffer(
        tex_t *t_tex, const char *t_tex_data, uint32_t width, uint32_t height, GLenum t_texture_unit,
        uint32_t t_channel_count
);

int bind_tex(tex_t *t_tex);

/**
 * Unbinds the current texture.
 */
int unbind_tex();

int delete_tex(tex_t *t_tex);

#endif //NM_OPENGL_H