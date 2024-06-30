#version 330

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;

out vec2 uv;

void main() {
	uv = inUv;

   gl_Position = vec4(inPos, 1.0);
}
