#version 410 core
#include "../common/lights.glsl"

in VertexDataOut {
    vec3 ws_position;
    vec3 ws_normal;
    vec2 texture_coordinate;
} frag_in;

layout(location = 0) out vec4 out_colour;

// Material properties
uniform vec3 diffuse_tint;
uniform vec3 specular_tint;
uniform vec3 ambient_tint;
uniform float shininess;

// Light Data
#if NUM_PL > 0
layout (std140) uniform PointLightArray {
    PointLightData point_lights[NUM_PL];
};
#endif

#if NUM_DL > 0
layout (std140) uniform DirectionalLightArray {
    DirectionalLightData directional_lights[NUM_DL];
};
#endif

// Global Data
uniform float inverse_gamma;
uniform vec3 ws_view_position;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_map_texture;

uniform float texture_scale; // Task E

void main() {
    // Scaled texture coordinates
    vec2 scaled_texture_coordinate = frag_in.texture_coordinate * texture_scale;

    // Per fragment lighting calculation
    vec3 ws_view_dir = normalize(ws_view_position - frag_in.ws_position);

    vec3 normalized_ws_normal = normalize(frag_in.ws_normal);

    LightCalculatioData light_calculation_data = LightCalculatioData(frag_in.ws_position, ws_view_dir, normalized_ws_normal);
    Material material = Material(diffuse_tint, specular_tint, ambient_tint, shininess);

    LightingResult lighting_result = total_light_calculation(light_calculation_data, material
        #if NUM_PL > 0
        ,point_lights
        #endif
        #if NUM_DL > 0
        ,directional_lights
        #endif
    );

    // Resolve lighting with fragment texture sampling
    vec3 resolved_lighting = resolve_textured_light_calculation(lighting_result, diffuse_texture, specular_map_texture, scaled_texture_coordinate);

    out_colour = vec4(resolved_lighting, 1.0f);
    out_colour.rgb = pow(out_colour.rgb, vec3(inverse_gamma));
}
