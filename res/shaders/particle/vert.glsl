#version 330 core

layout (location = 0) in vec3 a_position;    // World-space position of the vertex (calculated on CPU for billboarding)
layout (location = 1) in vec2 a_tex_coord;
layout (location = 2) in vec4 a_color;       // Per-vertex color, potentially interpolated from particle color

out vec2 v_tex_coord;
out vec4 v_color;

uniform mat4 view_matrix;       // This will be an identity matrix
uniform mat4 projection_matrix; // This will have projection_view_matrix

void main() {
    // Apply combined projection and view matrix directly
    gl_Position = projection_matrix * vec4(a_position, 1.0);
    v_tex_coord = a_tex_coord;
    v_color = a_color;
} 