#version 330 core

layout (location = 0) in vec3 a_position;    // World-space position of the vertex (calculated on CPU for billboarding)
layout (location = 1) in float a_size;
layout (location = 2) in vec4 a_color;       // Per-vertex color, potentially interpolated from particle color

out vec4 v_color;

uniform mat4 projection_view_matrix;

void main() {
    gl_Position = projection_view_matrix * vec4(a_position, 1.0);

    // Calculate point size based on perspective and particle size
    float distance_to_camera = gl_Position.w;
    if (distance_to_camera < 0.01) distance_to_camera = 0.01;
    gl_PointSize = a_size / distance_to_camera;

    gl_PointSize = max(1.0, a_size / distance_to_camera);

    v_color = a_color;
} 