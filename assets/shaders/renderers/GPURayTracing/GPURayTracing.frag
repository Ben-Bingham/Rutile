#version 330 core

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Object {
    mat4 model;
    mat4 invModel;
    int materialIndex;
};

struct Material {
    vec3 color;
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

const int MAX_MATERIALS = 50;
uniform Material materialBank[MAX_MATERIALS];

const float MAX_FLOAT = 3.402823466e+38F;

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
    float closestDistance = MAX_FLOAT;
    int hitObjectIndex;
    vec3 normal;

    for (int i = 0; i < objectCount; ++i) {
        vec3 o = (objects[i].invModel * vec4(ray.origin.xyz, 1.0)).xyz;
        vec3 d = (objects[i].invModel * vec4(ray.direction.xyz, 0.0)).xyz;

        vec3 co = spherePos - o;
        float a = dot(d, d);
        float b = -2.0 * dot(d, co);
        float c = dot(co, co) - (r * r);

        float discriminant = (b * b) - (4.0f * a * c);

        if (discriminant < 0.0) {
            continue;
        }

        float sqrtDiscriminant = sqrt(discriminant);

        // Because we subtract the discriminant, this root will always be smaller than the other one
        float t = (-b - sqrtDiscriminant) / (2.0 * a);

        if (t < 0.0) {
            t = (-b + sqrtDiscriminant) / (2.0 * a);
            if (t < 0.0) {
                continue;
            }
        }

        // At this point, no matter what t will be the closest hit for this object

        if (t < closestDistance) {
            closestDistance = t;
            hitSomething = true;
            hitObjectIndex = i;

            vec3 hitPointLocalSpace = o + t * d;

            normal = normalize(hitPointLocalSpace - spherePos);

            // Transform normal back to world space
            vec3 normalWorldSpace = transpose(inverse(mat3(objects[i].model))) * normal;
            normal = normalize(normalWorldSpace);
        }
    }

    if (hitSomething) {
          return (normal + 1.0) * 0.5;
        //return materialBank[objects[hitObjectIndex].materialIndex].color;
    }

    return backgroundColor;
}