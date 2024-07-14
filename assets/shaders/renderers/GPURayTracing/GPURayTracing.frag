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

uniform float s;

const float PI = 3.14159265359;

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

// Random Functions Keep seeds fractional, and ruffly in [0, 10]
float RandomFloat(float seed);
float RandomFloat(float seed, float low, float high); // Generates a number in the range [min, max)

vec3 RandomVec3(float seed);
vec3 RandomVec3(float seed, float low, float high);

vec3 RandomVec3InUnitSphere(float seed);
vec3 RandomUnitVec3(float seed); // Returns a normalized vec3 on the surface of the unit sphere
vec3 RandomVec3InHemisphere(float seed, vec3 normal);

vec2 randomState;

void main() {
    randomState = normalizedPixelPosition.xy * miliTime;

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
    float r = 1.0; // Sphere radius in local space
    vec3 spherePos = { 0.0, 0.0, 0.0 }; // Sphere position in local space

    float pixelColorMultiplier = 1.0;

    for (int i = 0; i < 2; ++i) {
        bool hitSomething = false;
        float closestDistance = MAX_FLOAT;
        int hitObjectIndex;
        vec3 hitNormal;
        vec3 hitPosition;

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

            if (t <= 0.001 || t >= MAX_FLOAT) {
                t = (-b + sqrtDiscriminant) / (2.0 * a);
                if (t <= 0.001 || t >= MAX_FLOAT) {
                    continue;
                }
            }

            // At this point, no matter what t will be the closest hit for this object

            if (t < closestDistance) {
                closestDistance = t;
                hitSomething = true;
                hitObjectIndex = i;

                vec3 hitPointLocalSpace = o + t * d;

                hitNormal = normalize(hitPointLocalSpace - spherePos);

                // Transform normal back to world space
                vec3 normalWorldSpace = transpose(inverse(mat3(objects[i].model))) * hitNormal;
                hitNormal = normalize(normalWorldSpace);

                hitPosition = (objects[i].model * vec4(hitPointLocalSpace, 1.0)).xyz;

            }
        }

        if (hitSomething) {
            pixelColorMultiplier *= 0.5;

            ray.origin = hitPosition;
            ray.direction = RandomVec3InHemisphere(1.434 * i * 0.91324, hitNormal);
        } else {
            break;
        }
    }

    return backgroundColor * pixelColorMultiplier;
}

const float PHI = 1.61803398874989484820459; 

float RandomFloat(float seed) {
    randomState.x = fract(sin(dot(randomState.xy, vec2(12.9898, 78.233))) * 43758.5453);
    randomState.y = fract(sin(dot(randomState.xy, vec2(12.9898, 78.233))) * 43758.5453);;
    
    return randomState.x;
}

float RandomFloat(float seed, float low, float high) {
    return low + (high - low) * RandomFloat(seed);
}

vec3 RandomVec3(float seed) {
    return vec3(RandomFloat(seed * 1.14634233), RandomFloat(seed * 0.931454), RandomFloat(seed * 1.04521));
}

vec3 RandomVec3(float seed, float low, float high) {
    return vec3(RandomFloat(seed * 1.92459083, low, high), RandomFloat(seed * 0.93298474, low, high), RandomFloat(seed * 1.248902, low, high));
}

vec3 RandomVec3InUnitSphere(float seed) {
    // This function was taken from:
    //https://github.com/riccardoprosdocimi/real-time-ray-tracer/blob/master/shaders/frag.glsl
	vec3 randomVector = RandomVec3(seed);
	float phi = 2.0 * PI * randomVector.x;
	float cosTheta = 2.0 * randomVector.y - 1.0;
	float u = randomVector.z;
    
	float theta = acos(cosTheta);
	float r = pow(u, 1.0 / 3.0);
    
	float x = r * sin(theta) * cos(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(theta);
    
	return vec3(x, y, z);
}

vec3 RandomUnitVec3(float seed) {
    return normalize(RandomVec3InUnitSphere(seed));
}

vec3 RandomVec3InHemisphere(float seed, vec3 normal) {
    vec3 unitSphere = RandomUnitVec3(seed);
    if (dot(unitSphere, normal) > 0.0) { // In the same hemisphere as the normal
        return unitSphere;
    }
    else {
        return -unitSphere;
    }
}