#version 330

uniform samplerCube cubeMap;

out vec4 outFragColor;

in vec2 uv;

uniform float horizontalModifier;
uniform float verticalModifier;

void main() {
    float phi = uv.x * 3.1415 * 2;
    float theta = (-uv.y + 0.5) * 3.1415;

    phi += verticalModifier;
    theta += horizontalModifier;

    vec3 dir = vec3(cos(phi) * cos(theta), sin(theta), sin(phi) * cos(theta));

    float depth = texture(cubeMap, dir).r;

    outFragColor = vec4(vec3(depth), 1.0);
}
