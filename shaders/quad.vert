#version 460 core

layout(location=0) in vec3 coordinate;

out vec2 tex_coord;

void main() {
    gl_Position = vec4(coordinate, 1);
    tex_coord = 0.5 * (coordinate.xy + 1.0);
}
