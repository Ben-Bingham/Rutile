#version 330 core

out vec4 outFragColor;

uniform vec3 color;

void main() {
   outFragColor = vec4(color.r, color.g, color.b, 1.0);
}