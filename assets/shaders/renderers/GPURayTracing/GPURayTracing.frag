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

uniform float miliTime;

uniform int screenWidth;
uniform int screenHeight;

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

uniform sampler2D accumulationBuffer;

vec3 FireRayIntoScene(Ray ray);

// Keep seed fractional, and ruffly in [0, 10]
float RandomFloat(float seed);

void main() {
    //outFragColor = vec4(normalizedPixelPosition.xy, 0.0, 1.0);
    //return;

    vec2 normalizedPixelCoordinate = normalizedPixelPosition;

    float normalizedPixelWidth = 1.0 / float(screenWidth);
    float normalizedPixelHeight = 1.0 / float(screenHeight);

    normalizedPixelCoordinate.x += normalizedPixelWidth / 2.0;
    normalizedPixelCoordinate.y += normalizedPixelHeight / 2.0;

    float widthJitter = (RandomFloat(2.5436) - 0.5) * normalizedPixelWidth;
    float heightJitter = (RandomFloat(3.135) - 0.5) * normalizedPixelHeight;

    normalizedPixelCoordinate.x += widthJitter;
    normalizedPixelCoordinate.y += heightJitter;

    // Camera
	vec2 clipSpacePixelPosition = (normalizedPixelCoordinate * 2.0) - 1.0;

    vec4 target = invProjection * vec4(clipSpacePixelPosition.xy, 1.0, 1.0);

    Ray ray;
    ray.direction = vec3(invView * vec4(normalize(vec3(target) / target.w), 0));
    ray.origin = cameraPosition;

    vec3 pixelColor = FireRayIntoScene(ray);

    // Writing to accumulation buffer
    vec3 accumulationColor = texture(accumulationBuffer, normalizedPixelPosition).rgb;

    accumulationColor += pixelColor;

    outFragColor = vec4(accumulationColor.xyz, 1.0);
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
        return materialBank[objects[hitObjectIndex].materialIndex].color;
    }

    return backgroundColor;
}

const float PHI = 1.61803398874989484820459; 

float RandomFloat(float seed) {
    float s = fract(miliTime + 0.1 * seed);

    float x = normalizedPixelPosition.x * screenWidth;
    float y = normalizedPixelPosition.y * screenHeight;

    return fract(tan(distance(vec2(x, y) * PHI, vec2(x, y)) * s) * x);
}