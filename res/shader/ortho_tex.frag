// fragment shader, use with: 'ortho_tex.vert'
#version 330 core

uniform sampler2D tex_sampler;

in vec2 tex;

out vec4 frag_color;

void main() {
    frag_color = texture(tex_sampler, tex);
}