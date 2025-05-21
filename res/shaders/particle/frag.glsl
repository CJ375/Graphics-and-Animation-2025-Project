#version 330 core

in vec4 v_color;

out vec4 f_color;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distance_squared = dot(coord, coord);
    
    if (distance_squared > 0.25) {
        discard;
    }
    
    float softness = 0.05;
    float alpha = 1.0;
    if (distance_squared > 0.25 - softness) {
        alpha = (0.25 - distance_squared) / softness;
    }
    
    f_color = v_color;
    f_color.a *= alpha;
} 