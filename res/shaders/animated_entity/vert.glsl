#version 410 core
#include "../common/maths.glsl"

// Per vertex data
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;
layout(location = 3) in vec4 bone_weights;
layout(location = 4) in uvec4 bone_indices;

out VertexDataOut {
    vec3 ws_position;
    vec3 ws_normal;
    vec2 texture_coordinate;
} vertex_data_out;

// Per instance data
uniform mat4 model_matrix;

// Animation Data
uniform mat4 bone_transforms[BONE_TRANSFORMS];

// Global Data
uniform mat4 projection_view_matrix;

void main() {
    // Transform vertices
    float sum = dot(bone_weights, vec4(1.0f));

    mat4 bone_transform =
        bone_weights[0] * bone_transforms[bone_indices[0]]
        + bone_weights[1] * bone_transforms[bone_indices[1]]
        + bone_weights[2] * bone_transforms[bone_indices[2]]
        + bone_weights[3] * bone_transforms[bone_indices[3]]
        + (1.0f - sum) * mat4(1.0f);

    mat4 animation_matrix = model_matrix * bone_transform;
    mat3 normal_matrix = cofactor(animation_matrix);

    vec3 calculated_ws_position = (animation_matrix * vec4(vertex_position, 1.0f)).xyz;    vec3 ws_normal = normalize(normal_matrix * normal);
    vec3 calculated_ws_normal = normalize(normal_matrix * normal);
    
    // Pass data to fragment shader
    vertex_data_out.ws_position = calculated_ws_position;
    vertex_data_out.ws_normal = calculated_ws_normal;
    vertex_data_out.texture_coordinate = texture_coordinate;

    gl_Position = projection_view_matrix * vec4(calculated_ws_position, 1.0f);
}
