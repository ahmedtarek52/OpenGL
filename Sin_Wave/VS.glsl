#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform float theta; // Time variable

out vec3 frag_color;

void main() {
    float amplitude = 0.5;   // Height of the wave
    float frequency = 10.0; // Frequency of the sine wave

    vec3 pos = vertex_position;
    pos.y += amplitude * sin(frequency * pos.x + theta); // Sine wave on Y-axis
    gl_Position = vec4(pos, 1.0);
    frag_color = vertex_color;
}
