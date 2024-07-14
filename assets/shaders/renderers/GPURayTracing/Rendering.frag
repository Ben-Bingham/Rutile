#version 330 core

out vec4 outFragColor;

in vec2 normalizedPixelPosition;

uniform sampler2D accumulationBuffer;
uniform int frameCount;

void main() {
	vec3 accumulationValue = texture(accumulationBuffer, normalizedPixelPosition).rgb;

	accumulationValue /= float(frameCount);

    outFragColor = vec4(accumulationValue.rgb, 1.0);
}
