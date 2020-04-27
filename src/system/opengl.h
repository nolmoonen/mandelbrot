#ifndef NM_OPENGL_H
#define NM_OPENGL_H

#include <glad/glad.h>
#include <stdbool.h>
#include <linmath.h>

typedef struct {
    GLuint m_shader_program;
} shader_program_t;

/** Call to {delete_shader_program} is required if {EXIT_FAILURE} is returned. */
int create_shader_program(
        shader_program_t *t_shader_program,
        const char *t_vert_shader_text, size_t t_vert_shader_size,
        const char *t_frag_shader_text, size_t t_frag_shader_size
);

int delete_shader_program(shader_program_t *t_shader_program);

/**
 * If {@param t_is_vertex} is {@code true}, create vertex shader. Otherwise, create fragment shader.
 * Call to {delete_shader} is required if {EXIT_FAILURE} is returned. */
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

/**
 * Png file on disk.
 * Call to {delete_tex} is required if {EXIT_FAILURE} is returned. */
int create_tex_from_file_on_disk(
        tex_t *t_tex, GLenum t_texture_unit,
        const char *t_tex_file
);

/**
 * Png file in memory.
 * Call to {delete_tex} is required if {EXIT_FAILURE} is returned. */
int create_tex_from_file_on_mem(
        tex_t *t_tex, GLenum t_texture_unit,
        const unsigned char *t_tex_data, size_t t_tex_len, uint32_t t_channel_count
);

/**
 * Texture data in memory. (RGB or RGBA)
 * {t_data_channel_count} is the number of channels the data has,
 * {t_texture_channel_count} is the number of channels the created texture should have
 * Call to {delete_tex} is required if {EXIT_FAILURE} is returned. */
int create_tex_from_mem(
        tex_t *t_tex, GLenum t_texture_unit,
        const unsigned char *t_tex_data, uint32_t width, uint32_t height,
        uint32_t t_texture_channel_count, uint32_t t_data_channel_count
);

int bind_tex(tex_t *t_tex);

/** Unbinds the current texture. */
int unbind_tex();

int delete_tex(tex_t *t_tex);

typedef struct {
    GLuint vao;
    GLuint vbo_pos;
    GLuint vbo_tex;
} quad;

int create_quad(quad *p_quad);

int delete_quad(quad *p_quad);

int render_ortho_tex_quad(quad *p_quad, tex_t *p_tex, mat4x4 p_model);

#endif //NM_OPENGL_H