#version 460 core

in vec2 tex_coord;
out vec4 color;

uniform sampler2D render;

void main() {
    color = texture(render, tex_coord).rgba;
}
