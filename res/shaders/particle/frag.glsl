#version 330 core

in vec2 v_tex_coord;
in vec4 v_color; // Interpolated color from vertex shader

out vec4 f_color; // Output fragment color

uniform sampler2D particle_texture_sampler;

void main() {
    vec4 tex_color = texture(particle_texture_sampler, v_tex_coord);
    f_color = tex_color * v_color;

    // Basic alpha discard for fully transparent pixels from texture, if needed
    // if (f_color.a < 0.01) {
    //     discard;
    // }
} 