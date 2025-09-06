#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;

uniform mat4 mvp;
uniform mat4 model;

out vec3 normal;
out vec3 fragPosition;

// Shadow Maps
//out vec4 fragPositionInLightSpace;

//uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = mvp * vec4(inPos.x, inPos.y, inPos.z, 1.0);

	normal = inNormal;
	fragPosition = vec3(model * vec4(inPos, 1.0));

	// Cascading Shadow maps
	//fragPositionInLightSpace = lightSpaceMatrix * vec4(fragPosition, 1.0);
}
