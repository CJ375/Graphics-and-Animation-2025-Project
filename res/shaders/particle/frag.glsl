#version 330 core

in vec4 v_color;
in float v_rotation;

out vec4 f_color;

void main() {
    vec2 centered = gl_PointCoord - vec2(0.5);

    float angle = radians(v_rotation);
    float cosA = cos(angle);
    float sinA = sin(angle);

    vec2 rotated = vec2(
        cosA * centered.x - sinA * centered.y,
        sinA * centered.x + cosA * centered.y
    );
    vec2 coord = rotated + vec2(0.5);

    float distance_squared = dot(coord - vec2(0.5), coord - vec2(0.5));
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