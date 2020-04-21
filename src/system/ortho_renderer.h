#ifndef BOMBER_RENDERER_H
#define BOMBER_RENDERER_H

#include "opengl.h"
#include "orto_model.h"

int screen_clear();

int render_ortho_tex_quad(quad *p_quad, tex_t *p_tex, mat4x4 p_model);

#endif //BOMBER_RENDERER_H
