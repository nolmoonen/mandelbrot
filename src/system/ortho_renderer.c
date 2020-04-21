#include <stdlib.h>
#include <stdint.h>

#include "ortho_renderer.h"
#include "shader_manager.h"

int screen_clear() {
    glClear((uint32_t) GL_COLOR_BUFFER_BIT | (uint32_t) GL_DEPTH_BUFFER_BIT);

    return EXIT_SUCCESS;
}

int render_ortho_tex_quad(quad *p_quad, tex_t *p_tex, mat4x4 p_model) {
    // obtain the shader program from id, and use it
    shader_program_t shader_program;
    request_program(&shader_program, SHADER_DEFAULT);
    use_shader_program(&shader_program);

    set_mat4x4(&shader_program, "modelMatrix", p_model);

    // obtain the texture from id, and bind the texture
    bind_tex(p_tex);

    // draw the quad
    glBindVertexArray(p_quad->vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // unbind shader and texture
    unbind_tex();
    unuse_shader_program();

    return EXIT_SUCCESS;
}