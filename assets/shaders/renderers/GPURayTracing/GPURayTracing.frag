#version 430 core

// Lots of code translated from Ray Tracing In One Weekend:
// https://raytracing.github.io/books/RayTracingInOneWeekend.html

//#define STATS

struct Ray {
	vec3 origin;
	vec3 direction;
    vec3 inverseDirection;
};

#ifdef STATS
struct Stats {
    int bboxChecks;
    int sphereChecks;
    int triangleChecks;
    int meshChecks;
} stats;

uniform int maxBboxChecks;
uniform int maxSphereChecks;
uniform int maxTriangleChecks;
uniform int maxMeshChecks;
#endif

const int DIFFUSE_TYPE = 0;
const int MIRROR_TYPE = 1;
const int DIELECTRIC_TYPE = 2;
const int EMMISIVE_TYPE = 3;
const int ONE_WAY_MIRROR = 4;

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
    int BVHStartIndex;
};

struct AABB {
    vec3 minBound;
    vec3 maxBound;
};

struct TLASNode {
    float minX;
    float minY;
    float minZ;
    
    float maxX;
    float maxY;
    float maxZ;

    int node1Offset;
    int objectCount;
};

struct BLASNode {
    float minX;
    float minY;
    float minZ;
    
    float maxX;
    float maxY;
    float maxZ;

    int node1Offset;
    int triangleCount;
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

layout(std430, binding = 3) readonly buffer TLASBuffer {
    TLASNode TLASNodes[];
};

layout(std430, binding = 4) readonly buffer BLASBuffer {
    BLASNode BLASNodes[];
};

uniform int objectCount;

uniform int BVHStartIndex;

uniform int objectBVHStartIndex;

struct HitInfo {
    vec3 normal;
    float closestDistance;
    vec3 hitPosition;
    int hitObjectIndex;

    bool frontFace;
};

const float PI = 3.14159265359;

const float MIN_RAY_DISTANCE = 0.00007;

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
const int MAX_INT = 2147483647;

uniform sampler2D accumulationBuffer;

uniform int maxBounces;

vec3 FireRayIntoScene(Ray ray);

// Scene hitting functions
bool HitScene(Ray ray, inout HitInfo hitInfo);
bool HitSphere(Ray ray, int objectIndex, inout HitInfo hitInfo);

bool HitTriangle(Ray ray, int objectIndex, inout HitInfo hitInfo, vec3[3] triangle);
bool HitMesh(Ray ray, int objectIndex, inout HitInfo hitInfo);

bool HitAABB(Ray ray, AABB bbox, out float distanceToIntersection);

// Materials
struct ScatterInfo {
    Ray ray;
    vec3 throughput;
};

ScatterInfo DiffuseScatter     (ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i);
ScatterInfo MirrorScatter      (ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i);
ScatterInfo DielectricScatter  (ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i);
ScatterInfo OneWayMirrorScatter(ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i);

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
#ifdef STATS
    stats = Stats(0, 0, 0, 0);
#endif

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
    ray.inverseDirection = 1.0 / ray.direction;
    ray.origin = cameraPosition;

    vec3 pixelColor = FireRayIntoScene(ray);

    pixelColor = LinearToGamma(pixelColor);

    // Writing to accumulation buffer
    vec3 accumulationColor = texture(accumulationBuffer, normalizedPixelPosition).rgb;

#ifdef STATS
    float col;
    if (maxBboxChecks != -1) {
        col = float(stats.bboxChecks) / float(maxBboxChecks);
    } else if (maxSphereChecks != -1) {
        col = float(stats.sphereChecks) / float(maxSphereChecks);
    } else if (maxTriangleChecks != -1) {
        col = float(stats.triangleChecks) / float(maxTriangleChecks);
    } else if (maxMeshChecks != -1) {
        col = float(stats.meshChecks) / float(maxMeshChecks);
    }

    pixelColor = vec3(col, 0.0, 0.0);

    accumulationColor = pixelColor;
#else
    accumulationColor += pixelColor;
#endif
  
    outFragColor = vec4(accumulationColor.rgb, 1.0);
}

float reflectance(float cosine, float refractionIndex) {
    float r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

vec3 FireRayIntoScene(Ray r) {
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec3 throughput = vec3(1.0, 1.0, 1.0);

    Ray ray = r;

    int bounces = 0;
    while (true) {
        if (bounces >= maxBounces) {
            break;
        }

        HitInfo hitInfo;
        if (HitScene(ray, hitInfo)) { // Scatter the ray
            
            ScatterInfo scatterInfo;
            scatterInfo.ray = ray;
            scatterInfo.throughput = vec3(0.0, 0.0, 0.0);

            Material mat = materialBank[objects[hitInfo.hitObjectIndex].materialIndex];

            if (mat.type == DIFFUSE_TYPE) {
                scatterInfo = DiffuseScatter(scatterInfo, mat, hitInfo, bounces);
            }
            else if (mat.type == MIRROR_TYPE) {
                scatterInfo = MirrorScatter(scatterInfo, mat, hitInfo, bounces);
            } 
            else if (mat.type == DIELECTRIC_TYPE) {
                scatterInfo = DielectricScatter(scatterInfo, mat, hitInfo, bounces);
            }
            else if (mat.type == EMMISIVE_TYPE) {
                color += throughput * mat.color;
                break;
            }
            else if (mat.type == ONE_WAY_MIRROR) {
                scatterInfo = OneWayMirrorScatter(scatterInfo, mat, hitInfo, bounces);
            }

            scatterInfo.ray.origin = hitInfo.hitPosition;
            scatterInfo.ray.inverseDirection = 1.0 / scatterInfo.ray.direction;

            ray = scatterInfo.ray;
            throughput *= scatterInfo.throughput;
        }
        else { // Missed everything, stop collecting new color
            color += throughput * 1.0;
            color += backgroundColor * throughput;
            break;
        }

        ++bounces;
    }

    return color;
}

// Stack concept, and some optimzations taken from:
// https://github.com/SebLague/Ray-Tracing/tree/main

bool HitScene(Ray ray, inout HitInfo hitInfo) {
    hitInfo.closestDistance = MAX_FLOAT;
    bool hitSomething = false;
 
    int stack[32];
    int stackIndex = 1;
    
    stack[stackIndex] = BVHStartIndex;
    
    while (stackIndex > 0) {
        int nodeIndex = stack[stackIndex];
        --stackIndex;
    
        TLASNode node = TLASNodes[nodeIndex];

        if (node.objectCount > 0) { // Is a leaf node, has multiple objects
            for (int i = node.node1Offset; i < node.node1Offset + node.objectCount; ++i) {
                HitInfo backupHitInfo = hitInfo;
    
                int geoType = objects[i].geometryType;
                
                if (geoType == SPHERE_TYPE) {
                    if (HitSphere(ray, i, backupHitInfo)) {
                        hitInfo = backupHitInfo;
                        hitSomething = true;
                    }
                } else if (geoType == MESH_TYPE) {
                    if (HitMesh(ray, i, backupHitInfo)) {
                        hitInfo = backupHitInfo;
                        hitSomething = true;
                    }
                }
            }
    
        } else { // Is a branch node, its children are other nodes
            float distanceNode1 = MAX_FLOAT;
            vec3 minBoundN1 = vec3(TLASNodes[node.node1Offset].minX, TLASNodes[node.node1Offset].minY, TLASNodes[node.node1Offset].minZ);
            vec3 maxBoundN1 = vec3(TLASNodes[node.node1Offset].maxX, TLASNodes[node.node1Offset].maxY, TLASNodes[node.node1Offset].maxZ);

            AABB bbox = AABB(minBoundN1, maxBoundN1);
            bool hit1 = HitAABB(ray, bbox, distanceNode1);
            
            float distanceNode2 = MAX_FLOAT;
            vec3 minBoundN2 = vec3(TLASNodes[node.node1Offset + 1].minX, TLASNodes[node.node1Offset + 1].minY, TLASNodes[node.node1Offset + 1].minZ);
            vec3 maxBoundN2 = vec3(TLASNodes[node.node1Offset + 1].maxX, TLASNodes[node.node1Offset + 1].maxY, TLASNodes[node.node1Offset + 1].maxZ);

            AABB bbox2 = AABB(minBoundN2, maxBoundN2);
            bool hit2 = HitAABB(ray, bbox2, distanceNode2);
    
            bool nearestIs1 = distanceNode1 < distanceNode2;
            
            int closeIndex = nearestIs1 ? node.node1Offset : node.node1Offset + 1;
            int farIndex = nearestIs1 ? node.node1Offset + 1 : node.node1Offset;
            
            float closeDistance = nearestIs1 ? distanceNode1 : distanceNode2;
            float farDistance = nearestIs1 ? distanceNode2 : distanceNode1;
            
            bool closeHit = nearestIs1 ? hit1 : hit2;
            bool farHit = nearestIs1 ? hit2 : hit1;
            
            if (farHit && farDistance < hitInfo.closestDistance) {
                stack[stackIndex += 1] = farIndex;
            }
            
            if (closeHit && closeDistance < hitInfo.closestDistance) {
                stack[stackIndex += 1] = closeIndex;
            }
        }
    }
    
    return hitSomething;
}

vec3 getFaceNormal(Ray ray, vec3 outwardNormal) {
    bool frontFace = dot(ray.direction, outwardNormal) < 0;
    vec3 normal = frontFace ? outwardNormal : -outwardNormal;

    return normal;
}

bool HitSphere(Ray ray, int objectIndex, inout HitInfo hitInfo) {
#ifdef STATS
    ++stats.sphereChecks;
#endif

    //float r = 1.0; // Sphere radius in local space
    //vec3 spherePos = { 0.0, 0.0, 0.0 }; // Sphere position in local space

    Object object = objects[objectIndex];

    // Transform the ray into the local space of the object
    vec3 o = (object.invModel * vec4(ray.origin.xyz, 1.0)).xyz;

    vec3 d = (object.invModel * vec4(normalize(ray.direction.xyz), 0.0)).xyz; // TODO pick a direction transformation
    //vec3 d = mat3(object.transposeInverseInverseModel) * normalize(ray.direction.xyz);
    d = normalize(d);

    // Intersection test
    vec3 co = -o; // Should be: vec3 co = spherePos - o;, but spherePos is (0.0, 0.0, 0.0)

    //float a = 1.0;
    float h = dot(d, co);
    float c = dot(co, co) - 1.0; // Should be: float c = dot(co, co) - r * r;, but radius is always 1.0
    
    float discriminant = h * h - 1.0 * c;

    if (discriminant < 0.0) {
        return false;
    }

    float sqrtDiscriminant = sqrt(discriminant);

    // Because we subtract the discriminant, this root will always be smaller than the other one
    float t = h - sqrtDiscriminant; // Should be: float t = (h - sqrtDiscriminant) / a;, but a is 1.0

    // Both t values are in the LOCAL SPACE of the object, so they can be compared to each other,
    // but they cannot be compared to the t values of other objects
    if (t <= MIN_RAY_DISTANCE || t >= MAX_FLOAT) {
        t = h + sqrtDiscriminant; // Should be: t = (h + sqrtDiscriminant) / a;, but a is 1.0
        if (t <= MIN_RAY_DISTANCE || t >= MAX_FLOAT) {
            return false;
        }
    }

    // At this point, no matter what t will be the closest hit for THIS object

    // Here we calculate the WORLD SPACE distance between the hit point and the ray for THIS object,
    // this can than be compared against other objects
    vec3 hitPointWorldSpace = (object.model * vec4(o + t * normalize(d), 1.0)).xyz;

    float lengthAlongRayWorldSpace = length(hitPointWorldSpace - ray.origin);

    if (lengthAlongRayWorldSpace < hitInfo.closestDistance) {
        hitInfo.closestDistance = lengthAlongRayWorldSpace;
        hitInfo.hitObjectIndex = objectIndex;

        vec3 hitPointLocalSpace = o + t * d;

        hitInfo.normal = normalize(hitPointLocalSpace); // Should be: outHitInfo.normal = normalize(hitPointLocalSpace - spherePos);, but spherePos is (0.0, 0.0, 0.0)

        vec3 outwardNormal = hitPointLocalSpace; // Should be: vec3 outwardNormal = (hitPointLocalSpace - spherePos) / r;, but sphere pos is 0 and r is 1
        hitInfo.frontFace = dot(ray.direction, outwardNormal) < 0.0;

        if (!hitInfo.frontFace) {
            hitInfo.normal = -hitInfo.normal;
        }

        // Transform normal back to world space
        vec3 normalWorldSpace = mat3(object.transposeInverseModel) * hitInfo.normal;
        hitInfo.normal = normalize(normalWorldSpace);

        hitInfo.hitPosition = (object.model * vec4(hitPointLocalSpace, 1.0)).xyz;

        return true;
    }

    return false;
}

bool IsInterior(float alpha, float beta) {
    return alpha > 0 && beta > 0 && alpha + beta < 1;
}

bool HitMesh(Ray ray, int objectIndex, inout HitInfo hitInfo) {
#ifdef STATS
    ++stats.meshChecks;
#endif

    bool hitSomething = false;

    int stack[32];
    int stackIndex = 1;

    stack[stackIndex] = objects[objectIndex].BVHStartIndex;

    while (stackIndex > 0) {
        int nodeIndex = stack[stackIndex];
        --stackIndex;

        BLASNode node = BLASNodes[nodeIndex];

        if (node.triangleCount > 0) { // Is a leaf node, has triangles
            for (int i = node.node1Offset; i < node.node1Offset + node.triangleCount; i += 9) {
                HitInfo backupHitInfo = hitInfo;
            
                vec3 v1 = vec3(meshData[i + 0], meshData[i + 1], meshData[i + 2]);
                vec3 v2 = vec3(meshData[i + 3], meshData[i + 4], meshData[i + 5]);
                vec3 v3 = vec3(meshData[i + 6], meshData[i + 7], meshData[i + 8]);
            
                vec3 triangle[3] = vec3[3](v1, v2 - v1, v3 - v1); // TODO move to CPU
            
                if(HitTriangle(ray, objectIndex, backupHitInfo, triangle)) {
                    if (backupHitInfo.closestDistance < hitInfo.closestDistance) {
                        hitInfo = backupHitInfo;
                        hitSomething = true;
                    }
                }
            }

        } else { // Is a branch node, its children are other nodes
            float distanceNode1 = MAX_FLOAT;
            vec3 minBoundN1 = vec3(BLASNodes[node.node1Offset].minX, BLASNodes[node.node1Offset].minY, BLASNodes[node.node1Offset].minZ);
            vec3 maxBoundN1 = vec3(BLASNodes[node.node1Offset].maxX, BLASNodes[node.node1Offset].maxY, BLASNodes[node.node1Offset].maxZ);
            
            vec3 o = (objects[objectIndex].invModel * vec4(ray.origin.xyz, 1.0)).xyz;

            vec3 d = (objects[objectIndex].invModel * vec4(normalize(ray.direction.xyz), 0.0)).xyz;
            //vec3 d = mat3(objects[objectIndex].transposeInverseInverseModel) * normalize(ray.direction.xyz);
            d = normalize(d);
            bool hit1 = HitAABB(Ray(o, d, normalize(1.0 / d)), AABB(minBoundN1, maxBoundN1), distanceNode1);

            distanceNode1 = (objects[objectIndex].model * vec4(distanceNode1, 0.0, 0.0, 0.0)).x;

            float distanceNode2 = MAX_FLOAT;
            vec3 minBoundN2 = vec3(BLASNodes[node.node1Offset + 1].minX, BLASNodes[node.node1Offset + 1].minY, BLASNodes[node.node1Offset + 1].minZ);
            vec3 maxBoundN2 = vec3(BLASNodes[node.node1Offset + 1].maxX, BLASNodes[node.node1Offset + 1].maxY, BLASNodes[node.node1Offset + 1].maxZ);
            bool hit2 = HitAABB(Ray(o, d, normalize(1.0 / d)), AABB(minBoundN2, maxBoundN2), distanceNode2);

            distanceNode2 = (objects[objectIndex].model * vec4(distanceNode2, 0.0, 0.0, 0.0)).x;

            bool nearestIs1 = distanceNode1 < distanceNode2;
            
            int closeIndex = nearestIs1 ? node.node1Offset : node.node1Offset + 1;
            int farIndex = nearestIs1 ? node.node1Offset + 1 : node.node1Offset;
            
            float closeDistance = nearestIs1 ? distanceNode1 : distanceNode2;
            float farDistance = nearestIs1 ? distanceNode2 : distanceNode1;
            
            bool closeHit = nearestIs1 ? hit1 : hit2;
            bool farHit = nearestIs1 ? hit2 : hit1;
            
            if (farHit && farDistance < hitInfo.closestDistance) {
                stack[stackIndex += 1] = farIndex;
            }
            
            if (closeHit && closeDistance < hitInfo.closestDistance) {
                stack[stackIndex += 1] = closeIndex;
            }
        }
    }
    
    return hitSomething;
}

bool HitTriangle(Ray ray, int objectIndex, inout HitInfo hitInfo, vec3 triangle[3]) {
#ifdef STATS
    ++stats.triangleChecks;
#endif

    Object object = objects[objectIndex];

    // Transform the ray into the local space of the object

    //vec3 o = ray.origin;
    //vec3 d = ray.direction;


    vec3 o = (object.invModel * vec4(ray.origin.xyz, 1.0)).xyz;

    vec3 d = (object.invModel * vec4(normalize(ray.direction.xyz), 0.0)).xyz; // TODO pick a direction transformation
    //vec3 d = mat3(object.transposeInverseInverseModel) * normalize(ray.direction.xyz);
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
        return false;
    }

    float t = (D - dot(normal, o)) / denom;

    if (t < MIN_RAY_DISTANCE) {
        return false;
    }

    // t is the closest point in object space

    vec3 intersection = o + d * t;

    // This check happens in object space
    vec3 planarHitPoint = intersection - Q;
    float alpha = dot(w, cross(planarHitPoint, v));
    float beta = dot(w, cross(u, planarHitPoint));

    if (!IsInterior(alpha, beta)) {
        return false;
    }

    vec3 hitPointWorldSpace = (object.model * vec4(o + t * normalize(d), 1.0)).xyz;
    float lengthAlongRayWorldSpace = length(hitPointWorldSpace - ray.origin);

    if (lengthAlongRayWorldSpace < hitInfo.closestDistance) {
        hitInfo.closestDistance = lengthAlongRayWorldSpace;
        hitInfo.hitPosition = hitPointWorldSpace;
        hitInfo.hitObjectIndex = objectIndex;
        
        vec3 normalWorldSpace = normalize((object.transposeInverseModel * vec4(normal, 0.0)).xyz);
        
        bool frontFace = dot(ray.direction, normalWorldSpace) < 0.0;
        hitInfo.normal = frontFace ? normalWorldSpace : -normalWorldSpace;
        hitInfo.frontFace = frontFace;

        return true;
    }

    return false;
}

bool HitAABB(Ray ray, AABB bbox, out float distanceToIntersection) {
#ifdef STATS
    ++stats.bboxChecks;
#endif

    vec3 t0Temp = (bbox.minBound - ray.origin) * ray.inverseDirection;
    vec3 t1Temp = (bbox.maxBound - ray.origin) * ray.inverseDirection;

    vec3 t0 = min(t0Temp, t1Temp);
    vec3 t1 = max(t0Temp, t1Temp);

    float tNear = max(t0.x, max(t0.y, t0.z));
    float tFar = min(t1.x, min(t1.y, t1.z));

    distanceToIntersection = tNear;

    return tFar >= tNear && tFar > 0;
}

ScatterInfo DiffuseScatter(ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i) {
    scatterInfo.ray.direction = normalize(hitInfo.normal + RandomUnitVec3(1.434 * i));

    if (NearZero(scatterInfo.ray.direction)) {
        scatterInfo.ray.direction = hitInfo.normal;
    }

    scatterInfo.throughput = vec3(mat.color.rgb);

    return scatterInfo;
}

ScatterInfo MirrorScatter(ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i) {
    scatterInfo.ray.direction = normalize(reflect(scatterInfo.ray.direction, hitInfo.normal));
    scatterInfo.ray.direction = normalize(scatterInfo.ray.direction + ((RandomUnitVec3(0.53424 * i) * vec3(mat.fuzz))));

    if (dot(scatterInfo.ray.direction, hitInfo.normal) > 0) {
        scatterInfo.throughput = vec3(mat.color.rgb);
    }
    else {
        scatterInfo.throughput = vec3(0.0, 0.0, 0.0);
    }

    return scatterInfo;
}

ScatterInfo DielectricScatter(ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i) {
    scatterInfo.throughput = vec3(1.0, 1.0, 1.0);
    float ri = hitInfo.frontFace ? (1.0 / mat.indexOfRefraction) : mat.indexOfRefraction;
                
    float cosTheta = min(dot(-normalize(scatterInfo.ray.direction), hitInfo.normal), 1.0);
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
                
    bool cannotRefract = ri * sinTheta > 1.0;

    if (cannotRefract || reflectance(cosTheta, ri) > RandomFloat(0.4245 * i)) {
        // Must Reflect
        scatterInfo.ray.direction = normalize(reflect(normalize(scatterInfo.ray.direction), normalize(hitInfo.normal)));
    } else {
        // Can Refract
        scatterInfo.ray.direction = normalize(refract(normalize(scatterInfo.ray.direction), normalize(hitInfo.normal), ri));
    }

    return scatterInfo;
}

ScatterInfo OneWayMirrorScatter(ScatterInfo scatterInfo, Material mat, HitInfo hitInfo, int i) {
    if (hitInfo.frontFace) {
        scatterInfo.ray.direction = normalize(reflect(scatterInfo.ray.direction, hitInfo.normal));
        scatterInfo.ray.direction = normalize(scatterInfo.ray.direction + ((RandomUnitVec3(0.53424 * i) * vec3(mat.fuzz))));
    
        if (dot(scatterInfo.ray.direction, hitInfo.normal) > 0) {
            scatterInfo.throughput = vec3(mat.color.rgb);
        }
        else {
            scatterInfo.throughput = vec3(1.0, 1.0, 1.0);
        }
    }

    scatterInfo.throughput = vec3(1.0, 1.0, 1.0);

    return scatterInfo;
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
