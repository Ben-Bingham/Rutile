#version 460 core
    
layout(triangles, invocations = 10) in; // TODO this 10 should be however many cascades we have curently
layout(triangle_strip, max_vertices = 3) out;

const int MAX_CASCADE_COUNT = 10;

uniform mat4 lightSpaceMatrices[MAX_CASCADE_COUNT];

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}