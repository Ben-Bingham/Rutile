#version 330 core

layout (location = 0) in vec3 inPos;

out vec2 normalizedPixelPosition;

void main() {
   gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);

   normalizedPixelPosition.xy = inPos.xy * 0.5 + 0.5;
}
