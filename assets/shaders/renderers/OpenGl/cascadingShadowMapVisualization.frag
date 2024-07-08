#version 330

uniform sampler2DArray shadowMap;
uniform int layer;

out vec4 outFragColor;

in vec2 uv;

void main() {
    outFragColor = vec4(texture(shadowMap, vec3(uv.xy, layer)).xyz, 1.0);
}
