// vertex shader, use with 'ortho_tex.frag'
#version 330 core

uniform mat4 modelMatrix;

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;

out vec2 tex;

void main(){
    gl_Position = modelMatrix * vec4(inPos, 0.0, 1.0);
    tex = inTex;
}