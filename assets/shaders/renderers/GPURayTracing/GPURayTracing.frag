#version 430 core

// Lots of code translated from Ray Tracing In One Weekend:
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

struct Ray {
	vec3 origin;
	vec3 direction;
};

const int DIFFUSE_TYPE = 0;
const int MIRROR_TYPE = 1;
const int DIELECTRIC_TYPE = 2;
const int EMMISIVE_TYPE = 3;

struct Material {
    int type;
    float fuzz;
    float indexOfRefraction;
    vec3 color;
};

const int SPHERE_TYPE = 0;
const int MESH_TYPE = 1;

struct Object {
    mat4 model;
    mat4 invModel;

    mat4 transposeInverseModel;        // transpose(inverse(model));
    mat4 transposeInverseInverseModel; // transpose(inverse(invModel));

    int materialIndex;

    int geometryType;
    int meshOffset;
    int meshSize;
};

layout(std430, binding = 0) readonly buffer materialBuffer {
    Material materialBank[];
};

layout(std430, binding = 1) readonly buffer objectBuffer {
    Object objects[];
};

layout(std430, binding = 2) readonly buffer meshBuffer {
    float meshData[];
};

uniform int objectCount;

struct HitInfo {
    bool hitSomething;

    vec3 normal;
    float closestDistance;
    vec3 hitPosition;
    int hitObjectIndex;

    bool frontFace;
};

const float PI = 3.14159265359;

const float MIN_RAY_DISTANCE = 0.00005;

uniform float miliTime;

uniform int screenWidth;
uniform int screenHeight;

out vec4 outFragColor;

in vec2 normalizedPixelPosition;

uniform mat4 invView;
uniform mat4 invProjection;

uniform vec3 cameraPosition;

uniform vec3 backgroundColor;

const float MAX_FLOAT = 3.402823466e+38F;

uniform sampler2D accumulationBuffer;

uniform int maxBounces;

vec3 FireRayIntoScene(Ray ray);

// Scene hitting functions
HitInfo HitScene(Ray ray);
HitInfo HitSphere(Ray ray, int objectIndex, HitInfo hitInfo);

HitInfo HitTriangle(Ray ray, int objectIndex, HitInfo hitInfo, vec3[3] triangle);
HitInfo HitMesh(Ray ray, int objectIndex, HitInfo hitInfo);

// Random Functions Keep seeds fractional, and ruffly in [0, 10]
float RandomFloat(float seed);
float RandomFloat(float seed, float low, float high); // Generates a number in the range [min, max)

vec3 RandomVec3(float seed);
vec3 RandomVec3(float seed, float low, float high);

vec3 RandomVec3InUnitSphere(float seed);
vec3 RandomUnitVec3(float seed); // Returns a normalized vec3 on the surface of the unit sphere
vec3 RandomVec3InHemisphere(float seed, vec3 normal);

vec3 RandomInUnitDisk(float seed);

vec2 randomState;

float LinearToGamma(float component) {
    if (component > 0.0f) {
        return sqrt(component);
    }
    return 0.0f;
}

vec3 LinearToGamma(vec3 color) {
    return vec3(LinearToGamma(color.r), LinearToGamma(color.g), LinearToGamma(color.b));
}

bool NearZero(vec3 vec) {
    float epsilon = 1e-8;
    return (abs(vec.x) < epsilon) && (abs(vec.y) < epsilon) && (abs(vec.z) < epsilon);
}

void main() {
    randomState = normalizedPixelPosition.xy * miliTime * 3.4135;

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
    ray.direction = normalize(vec3(invView * vec4(normalize(vec3(target) / target.w), 0)));
    ray.origin = cameraPosition;

    vec3 pixelColor = FireRayIntoScene(ray);

    pixelColor = LinearToGamma(pixelColor);

    // Writing to accumulation buffer
    vec3 accumulationColor = texture(accumulationBuffer, normalizedPixelPosition).rgb;

    accumulationColor += pixelColor;

    outFragColor = vec4(accumulationColor.rgb, 1.0);
}

float reflectance(float cosine, float refractionIndex) {
    float r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

vec3 FireRayIntoScene(Ray ray) {
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec3 throughput = vec3(1.0, 1.0, 1.0);

    int bounces = 0;
    while (true) {
        vec3 scatterColor = vec3(0.0, 0.0, 0.0);

        if (bounces >= maxBounces) {
            break;
        }
        
        HitInfo hitInfo = HitScene(ray);

        if (hitInfo.hitSomething) { // Then scatter the ray
            ray.origin = hitInfo.hitPosition;

            Material mat = materialBank[objects[hitInfo.hitObjectIndex].materialIndex];

            if (mat.type == DIFFUSE_TYPE) {
                ray.direction = normalize(hitInfo.normal + RandomUnitVec3(1.434 * bounces));

                if (NearZero(ray.direction)) {
                    ray.direction = hitInfo.normal;
                }

                scatterColor = vec3(mat.color.rgb);
            } else if (mat.type == MIRROR_TYPE) {
                ray.direction = normalize(reflect(ray.direction, hitInfo.normal));
                ray.direction = normalize(ray.direction + ((RandomUnitVec3(0.53424 * bounces) * vec3(mat.fuzz))));

                if (dot(ray.direction, hitInfo.normal) > 0) {
                    scatterColor = vec3(mat.color.rgb);
                }
                else {
                    scatterColor = vec3(0.0, 0.0, 0.0);
                }

            } else if (mat.type == DIELECTRIC_TYPE) {
                scatterColor = vec3(1.0, 1.0, 1.0);
                float ri = hitInfo.frontFace ? (1.0 / mat.indexOfRefraction) : mat.indexOfRefraction;
                
                float cosTheta = min(dot(-normalize(ray.direction), hitInfo.normal), 1.0);
                float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
                
                bool cannotRefract = ri * sinTheta > 1.0;

                if (cannotRefract || reflectance(cosTheta, ri) > RandomFloat(0.4245 * bounces)) {
                    // Must Reflect
                    ray.direction = normalize(reflect(normalize(ray.direction), normalize(hitInfo.normal)));
                } else {
                    // Can Refract
                    ray.direction = normalize(refract(normalize(ray.direction), normalize(hitInfo.normal), ri));
                }
            } else if (mat.type == EMMISIVE_TYPE) {
                color += throughput * vec3(15.0, 15.0, 15.0);

            }
        } else { // Missed everything, stop collecting new color
            color += backgroundColor * throughput;
            break;
        }

        throughput *= scatterColor;

        ++bounces;
    }

    return color;
}

HitInfo HitScene(Ray ray) {
    HitInfo hitInfo;
    hitInfo.hitSomething = false;
    hitInfo.closestDistance = MAX_FLOAT;

    for (int i = 0; i < objectCount; ++i) {
        int geoType = objects[i].geometryType;

        if (geoType == SPHERE_TYPE) {
            hitInfo = HitSphere(ray, i, hitInfo);
        } else if (geoType == MESH_TYPE) {
            hitInfo = HitMesh(ray, i, hitInfo);
        }
    }

    return hitInfo;
}

vec3 getFaceNormal(Ray ray, vec3 outwardNormal) {
    bool frontFace = dot(ray.direction, outwardNormal) < 0;
    vec3 normal = frontFace ? outwardNormal : -outwardNormal;

    return normal;
}

HitInfo HitSphere(Ray ray, int objectIndex, HitInfo currentHitInfo) {
    //float r = 1.0; // Sphere radius in local space
    //vec3 spherePos = { 0.0, 0.0, 0.0 }; // Sphere position in local space

    Object object = objects[objectIndex];

    HitInfo outHitInfo = currentHitInfo;

    // Transform the ray into the local space of the object
    vec3 o = (object.invModel * vec4(ray.origin.xyz, 1.0)).xyz;

    vec3 d = (object.invModel * vec4(ray.direction.xyz, 0.0)).xyz; // TODO pick a direction transformation
    //vec3 d = mat3(object.transposeInverseInverseModel) * ray.direction.xyz;
    d = normalize(d);

    // Intersection test
    vec3 co = -o; // Should be: vec3 co = spherePos - o;, but spherePos is (0.0, 0.0, 0.0)

    //float a = 1.0;
    float h = dot(d, co);
    float c = dot(co, co) - 1.0; // Should be: float c = dot(co, co) - r * r;, but radius is always 1.0
    
    float discriminant = h * h - 1.0 * c;

    if (discriminant < 0.0) {
        return outHitInfo;
    }

    float sqrtDiscriminant = sqrt(discriminant);

    // Because we subtract the discriminant, this root will always be smaller than the other one
    float t = h - sqrtDiscriminant; // Should be: float t = (h - sqrtDiscriminant) / a;, but a is 1.0

    // Both t values are in the LOCAL SPACE of the object, so they can be compared to each other,
    // but they cannot be compared to the t values of other objects
    if (t <= MIN_RAY_DISTANCE || t >= MAX_FLOAT) {
        t = h + sqrtDiscriminant; // Should be: t = (h + sqrtDiscriminant) / a;, but a is 1.0
        if (t <= MIN_RAY_DISTANCE || t >= MAX_FLOAT) {
            return outHitInfo;
        }
    }

    // At this point, no matter what t will be the closest hit for THIS object

    // Here we calculate the WORLD SPACE distance between the hit point and the ray for THIS object,
    // this can than be compared against other objects
    vec3 hitPointWorldSpace = (object.model * vec4(o + t * normalize(d), 1.0)).xyz;

    float lengthAlongRayWorldSpace = length(hitPointWorldSpace - ray.origin);

    if (lengthAlongRayWorldSpace < currentHitInfo.closestDistance) {
        outHitInfo.closestDistance = lengthAlongRayWorldSpace;
        outHitInfo.hitSomething = true;
        outHitInfo.hitObjectIndex = objectIndex;

        vec3 hitPointLocalSpace = o + t * d;

        outHitInfo.normal = normalize(hitPointLocalSpace); // Should be: outHitInfo.normal = normalize(hitPointLocalSpace - spherePos);, but spherePos is (0.0, 0.0, 0.0)

        vec3 outwardNormal = hitPointLocalSpace; // Should be: vec3 outwardNormal = (hitPointLocalSpace - spherePos) / r;, but sphere pos is 0 and r is 1
        outHitInfo.frontFace = dot(ray.direction, outwardNormal) < 0.0;

        if (!outHitInfo.frontFace) {
            outHitInfo.normal = -outHitInfo.normal;
        }

        // Transform normal back to world space
        vec3 normalWorldSpace = mat3(object.transposeInverseModel) * outHitInfo.normal;
        outHitInfo.normal = normalize(normalWorldSpace);

        outHitInfo.hitPosition = (object.model * vec4(hitPointLocalSpace, 1.0)).xyz;
    }

    return outHitInfo;
}

bool IsInterior(float alpha, float beta) {
    return alpha > 0 && beta > 0 && alpha + beta < 1;
}

HitInfo HitMesh(Ray ray, int objectIndex, HitInfo hitInfo) {
    Object object = objects[objectIndex];
    
    HitInfo outHitInfo = hitInfo;
    
    for (int i = 0; i < object.meshSize; i += 9) {

        vec3 v1 = vec3(meshData[object.meshOffset + i + 0], meshData[object.meshOffset + i + 1], meshData[object.meshOffset + i + 2]);
        vec3 v2 = vec3(meshData[object.meshOffset + i + 3], meshData[object.meshOffset + i + 4], meshData[object.meshOffset + i + 5]);
        vec3 v3 = vec3(meshData[object.meshOffset + i + 6], meshData[object.meshOffset + i + 7], meshData[object.meshOffset + i + 8]);

        vec3 triangle[3] = vec3[3](v1, v2 - v1, v3 - v1);
    
        outHitInfo = HitTriangle(ray, objectIndex, outHitInfo, triangle);
    }

    return outHitInfo;
}

HitInfo HitTriangle(Ray ray, int objectIndex, HitInfo hitInfo, vec3 triangle[3]) {
    Object object = objects[objectIndex];

    // Transform the ray into the local space of the object
    vec3 o = (object.invModel * vec4(ray.origin.xyz, 1.0)).xyz;

    vec3 d = (object.invModel * vec4(ray.direction.xyz, 0.0)).xyz; // TODO pick a direction transformation
    //vec3 d = mat3(object.transposeInverseInverseModel) * ray.direction.xyz;
    d = normalize(d);

    // Quad definition
    vec3 Q = triangle[0];
    vec3 u = triangle[1];
    vec3 v = triangle[2];

    vec3 n = cross(u, v);
    vec3 normal = normalize(n);
    float D = dot(normal, Q);
    vec3 w = n / dot(n, n);

    // Plane intersection
    float denom = dot(normal, d);
    if (abs(denom) < 1e-8) {
        return hitInfo;
    }

    float t = (D - dot(normal, o)) / denom;

    if (t < MIN_RAY_DISTANCE) {
        return hitInfo;
    }

    // t is the closest point in object space

    vec3 intersection = o + d * t;

    // This check happens in object space
    vec3 planarHitPoint = intersection - Q;
    float alpha = dot(w, cross(planarHitPoint, v));
    float beta = dot(w, cross(u, planarHitPoint));

    if (!IsInterior(alpha, beta)) {
        return hitInfo;
    }

    HitInfo outHitInfo = hitInfo;

    vec3 hitPointWorldSpace = (object.model * vec4(o + t * normalize(d), 1.0)).xyz;
    float lengthAlongRayWorldSpace = length(hitPointWorldSpace - ray.origin);

    if (lengthAlongRayWorldSpace < hitInfo.closestDistance) {
        outHitInfo.closestDistance = lengthAlongRayWorldSpace;
        outHitInfo.hitSomething = true;
        outHitInfo.hitObjectIndex = objectIndex;

        vec3 normalWorldSpace = mat3(object.transposeInverseModel) * normal;
        outHitInfo.normal = normalize(getFaceNormal(Ray(o, d), normalWorldSpace));

        outHitInfo.hitPosition = hitPointWorldSpace;
    }

    return outHitInfo;
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

vec3 RandomInUnitDisk(float seed) {
    int i = 0;
    while (true) {
        vec3 p = vec3(RandomFloat(0.4234 * i, -1.0, 1.0), RandomFloat(1.590 * i, -1.0, 1.0), 0.0);
        if (p.x * p.x + p.y * p.y + p.z * p.z < 1.0) {
            return p;
        }

        ++i;
    }
}
