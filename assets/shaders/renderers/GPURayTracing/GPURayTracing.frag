#version 330 core

out vec4 outFragColor;

in vec2 normalizedPixelPosition;

void main() {
   outFragColor = vec4(normalizedPixelPosition.xy, 0.0, 1.0);
}
