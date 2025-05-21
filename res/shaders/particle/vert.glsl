#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in float a_size;
layout (location = 2) in vec4 a_color;
layout (location = 3) in float a_rotation;

out vec4 v_color;
out float v_rotation;

uniform mat4 projection_view_matrix;

void main() {
    gl_Position = projection_view_matrix * vec4(a_position, 1.0);

    // Calculate point size based on perspective and particle size
    float distance_to_camera = gl_Position.w;
    if (distance_to_camera < 0.01) distance_to_camera = 0.01;
    gl_PointSize = a_size / distance_to_camera;

    gl_PointSize = max(1.0, a_size / distance_to_camera);

    v_rotation = a_rotation;
    v_color = a_color;
} 