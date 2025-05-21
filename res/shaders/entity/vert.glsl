#version 410 core
#include "../common/maths.glsl"

// Per vertex data
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;

out VertexDataOut {
    vec3 ws_position;
    vec3 ws_normal;
    vec2 texture_coordinate;
} vertex_data_out;

// Per instance data
uniform mat4 model_matrix;
uniform mat3 normal_matrix;

// Global data
uniform mat4 projection_view_matrix;

void main() {
    // Transform vertices
    vec3 calculated_ws_position = (model_matrix * vec4(vertex_position, 1.0f)).xyz;

    vec3 calculated_ws_normal = normalize(normal_matrix * normal); 

    // Pass data to fragment shader
    vertex_data_out.ws_position = calculated_ws_position;
    vertex_data_out.ws_normal = calculated_ws_normal;
    vertex_data_out.texture_coordinate = texture_coordinate;

    gl_Position = projection_view_matrix * vec4(calculated_ws_position, 1.0f);
}
