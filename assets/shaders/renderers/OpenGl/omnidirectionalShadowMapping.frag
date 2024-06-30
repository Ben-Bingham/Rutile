#version 330 core
in vec4 fragmentPosition;

uniform vec3 lightPosition;
uniform float farPlane;

void main() {
    //gl_FragDepth = 0.5;
    // get distance between fragment and light source
    float lightDistance = length(fragmentPosition.xyz - lightPosition);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  