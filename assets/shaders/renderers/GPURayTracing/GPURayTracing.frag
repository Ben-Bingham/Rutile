#version 330 core

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Object {
    mat4 invModel;
};

out vec4 outFragColor;

in vec2 normalizedPixelPosition;

uniform mat4 invView;
uniform mat4 invProjection;

uniform vec3 cameraPosition;

uniform vec3 backgroundColor;

const int MAX_OBJECTS = 50;
uniform Object objects[MAX_OBJECTS];
uniform int objectCount;

vec3 FireRayIntoScene(Ray ray);

void main() {
    // Camera
	vec2 clipSpacePixelPosition = (normalizedPixelPosition * 2.0) - 1.0;

    vec4 target = invProjection * vec4(clipSpacePixelPosition.xy, 1.0, 1.0);

    Ray ray;
    ray.direction = vec3(invView * vec4(normalize(vec3(target) / target.w), 0));
    ray.origin = cameraPosition;

    vec3 pixelColor = FireRayIntoScene(ray);

    outFragColor = vec4(pixelColor.xyz, 1.0);
}

vec3 FireRayIntoScene(Ray ray) {
    float r = 1.0f; // Sphere radius in local space
    vec3 spherePos = { 0.0, 0.0, 0.0 }; // Sphere position in local space

    bool hitSomething = false;

    for (int i = 0; i < objectCount; ++i) {
        vec3 o = (objects[i].invModel * vec4(ray.origin.xyz, 1.0)).xyz;
        vec3 d = (objects[i].invModel * vec4(ray.direction.xyz, 0.0)).xyz;

        vec3 co = spherePos - o;
        float a = dot(d, d);
        float b = -2.0 * dot(d, co);
        float c = dot(co, co) - (r * r);

        float discriminant = (b * b) - (4.0f * a * c);

        if (discriminant >= 0.0f) {
            hitSomething = true;
            break;
        }
    }

    if (hitSomething) {
        return vec3(0.0, 1.0, 0.0);
    }

    return backgroundColor;
}