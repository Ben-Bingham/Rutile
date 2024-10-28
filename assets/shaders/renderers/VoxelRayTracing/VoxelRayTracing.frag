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

struct AABB {
    vec3 minBound;
    vec3 maxBound;
};


layout(std430, binding = 0) readonly buffer materialBuffer {
    Material materialBank[];
};

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

bool HitAABB(Ray ray, AABB bbox, out float distanceToIntersection);
bool HitAABB2(Ray ray, AABB bbox, inout HitInfo hitInfo);
bool HitAABB3(Ray ray, AABB bbox, inout float dist);


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
    if (component > 0.0) {
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








            //return vec3(0.0, 1.0, 0.0);










            ScatterInfo scatterInfo;
            scatterInfo.ray = ray;
            scatterInfo.throughput = vec3(0.0, 0.0, 0.0);

            // Material mat = materialBank[objects[hitInfo.hitObjectIndex].materialIndex]; TODO
            Material mat;
            mat.type = DIFFUSE_TYPE;
            mat.color = vec3(0.35, 0.75, 0.4);
            mat.fuzz = 0.5;
            mat.indexOfRefraction = 1.0;

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





    //return vec3(1.0, 0.0, 0.0);




    return color;
}

// Stack concept, and some optimzations taken from:
// https://github.com/SebLague/Ray-Tracing/tree/main

struct Voxel {
    float minX;
    float minY;
    float minZ;
    
    float maxX;
    float maxY;
    float maxZ;

    int k0;
    int k1;
    int k2;
    int k3;
    int k4;
    int k5;
    int k6;
    int k7;

    bool hasKids;
    bool shouldDraw;
};

layout(std430, binding = 5) readonly buffer VoxelBuffer {
    Voxel voxels[];
};

uniform int octreeRootIndex;

struct DistIndex {
    float dist;
    int index;
};

struct DistIndexHitVoxelInd {
    float dist;
    int index;
    bool hit;
    int voxelIndex;
};

bool HitScene(Ray ray, inout HitInfo hitInfo) {
    hitInfo.closestDistance = MAX_FLOAT;
    bool hitSomething = false;
 
    Voxel rootVoxel = voxels[octreeRootIndex];
    float dist;
    if (!HitAABB(ray, AABB(vec3(rootVoxel.minX, rootVoxel.minY, rootVoxel.minZ), vec3(rootVoxel.maxX, rootVoxel.maxY, rootVoxel.maxZ)), dist)) {
        return false;
    }

    DistIndex stack[128];
    int stackIndex = 1;
    
    DistIndex rootElement;
    rootElement.index = octreeRootIndex;
    rootElement.dist = dist;

    stack[stackIndex] = rootElement;
    
    while (stackIndex > 0) {
        DistIndex element = stack[stackIndex];
        --stackIndex;
    
        Voxel voxel = voxels[element.index];
        
        if (element.dist > hitInfo.closestDistance) {
            continue;
        }

        //// TODO Commenting/uncomenting this causes perforamce cchange TODO
        //float dist;
        //if (!HitAABB(ray, AABB(vec3(voxel.minX, voxel.minY, voxel.minZ), vec3(voxel.maxX, voxel.maxY, voxel.maxZ)), dist)) {
        //    continue;
        //}
        
        //if (dist > hitInfo.closestDistance) {
        //    continue;
        //}

        if (voxel.hasKids) {
            //if (voxel.k0 != -1) { stack[stackIndex += 1] = voxel.k0; }
            //if (voxel.k1 != -1) { stack[stackIndex += 1] = voxel.k1; }
            //if (voxel.k2 != -1) { stack[stackIndex += 1] = voxel.k2; }
            //if (voxel.k3 != -1) { stack[stackIndex += 1] = voxel.k3; }
            //if (voxel.k4 != -1) { stack[stackIndex += 1] = voxel.k4; }
            //if (voxel.k5 != -1) { stack[stackIndex += 1] = voxel.k5; }
            //if (voxel.k6 != -1) { stack[stackIndex += 1] = voxel.k6; }
            //if (voxel.k7 != -1) { stack[stackIndex += 1] = voxel.k7; }

            DistIndexHitVoxelInd distIndices[8];

            distIndices[0].dist = hitInfo.closestDistance;
            distIndices[1].dist = hitInfo.closestDistance;
            distIndices[2].dist = hitInfo.closestDistance;
            distIndices[3].dist = hitInfo.closestDistance;
            distIndices[4].dist = hitInfo.closestDistance;
            distIndices[5].dist = hitInfo.closestDistance;
            distIndices[6].dist = hitInfo.closestDistance;
            distIndices[7].dist = hitInfo.closestDistance;

            distIndices[0].index = 0;
            distIndices[1].index = 1;
            distIndices[2].index = 2;
            distIndices[3].index = 3;
            distIndices[4].index = 4;
            distIndices[5].index = 5;
            distIndices[6].index = 6;
            distIndices[7].index = 7;
            
            distIndices[0].hit = false;
            distIndices[1].hit = false;
            distIndices[2].hit = false;
            distIndices[3].hit = false;
            distIndices[4].hit = false;
            distIndices[5].hit = false;
            distIndices[6].hit = false;
            distIndices[7].hit = false;

            distIndices[0].voxelIndex = voxel.k0;
            distIndices[1].voxelIndex = voxel.k1;
            distIndices[2].voxelIndex = voxel.k2;
            distIndices[3].voxelIndex = voxel.k3;
            distIndices[4].voxelIndex = voxel.k4;
            distIndices[5].voxelIndex = voxel.k5;
            distIndices[6].voxelIndex = voxel.k6;
            distIndices[7].voxelIndex = voxel.k7;
            
            if (voxel.k0 != -1) { Voxel k0 = voxels[voxel.k0]; distIndices[0].hit = HitAABB3(ray, AABB(vec3(k0.minX, k0.minY, k0.minZ), vec3(k0.maxX, k0.maxY, k0.maxZ)), distIndices[0].dist); }
            if (voxel.k1 != -1) { Voxel k1 = voxels[voxel.k1]; distIndices[1].hit = HitAABB3(ray, AABB(vec3(k1.minX, k1.minY, k1.minZ), vec3(k1.maxX, k1.maxY, k1.maxZ)), distIndices[1].dist); }
            if (voxel.k2 != -1) { Voxel k2 = voxels[voxel.k2]; distIndices[2].hit = HitAABB3(ray, AABB(vec3(k2.minX, k2.minY, k2.minZ), vec3(k2.maxX, k2.maxY, k2.maxZ)), distIndices[2].dist); }
            if (voxel.k3 != -1) { Voxel k3 = voxels[voxel.k3]; distIndices[3].hit = HitAABB3(ray, AABB(vec3(k3.minX, k3.minY, k3.minZ), vec3(k3.maxX, k3.maxY, k3.maxZ)), distIndices[3].dist); }
            if (voxel.k4 != -1) { Voxel k4 = voxels[voxel.k4]; distIndices[4].hit = HitAABB3(ray, AABB(vec3(k4.minX, k4.minY, k4.minZ), vec3(k4.maxX, k4.maxY, k4.maxZ)), distIndices[4].dist); }
            if (voxel.k5 != -1) { Voxel k5 = voxels[voxel.k5]; distIndices[5].hit = HitAABB3(ray, AABB(vec3(k5.minX, k5.minY, k5.minZ), vec3(k5.maxX, k5.maxY, k5.maxZ)), distIndices[5].dist); }
            if (voxel.k6 != -1) { Voxel k6 = voxels[voxel.k6]; distIndices[6].hit = HitAABB3(ray, AABB(vec3(k6.minX, k6.minY, k6.minZ), vec3(k6.maxX, k6.maxY, k6.maxZ)), distIndices[6].dist); }
            if (voxel.k7 != -1) { Voxel k7 = voxels[voxel.k7]; distIndices[7].hit = HitAABB3(ray, AABB(vec3(k7.minX, k7.minY, k7.minZ), vec3(k7.maxX, k7.maxY, k7.maxZ)), distIndices[7].dist); }
            
#define CMP(a, b) distIndices[a].dist < distIndices[b].dist
#define SWAP(a, b) DistIndexHitVoxelInd t1 = distIndices[a]; distIndices[a] = distIndices[b]; distIndices[b] = t1;
#define CSWAP(a, b) if (CMP(a, b)) { SWAP(a, b); }

            CSWAP(0, 2); CSWAP(1, 3); CSWAP(4, 6); CSWAP(5, 7);
            CSWAP(0, 4); CSWAP(1, 5); CSWAP(2, 6); CSWAP(3, 7);
            CSWAP(0, 1); CSWAP(2, 3); CSWAP(4, 5); CSWAP(6, 7);
            CSWAP(2, 4); CSWAP(3, 5);
            CSWAP(1, 4); CSWAP(3, 6);
            CSWAP(1, 2); CSWAP(3, 4); CSWAP(5, 6);

            if (distIndices[0].hit) { DistIndex element; element.index = distIndices[0].voxelIndex; element.dist = distIndices[0].dist; stack[stackIndex += 1] = element; }
            if (distIndices[1].hit) { DistIndex element; element.index = distIndices[1].voxelIndex; element.dist = distIndices[1].dist; stack[stackIndex += 1] = element; }
            if (distIndices[2].hit) { DistIndex element; element.index = distIndices[2].voxelIndex; element.dist = distIndices[2].dist; stack[stackIndex += 1] = element; }
            if (distIndices[3].hit) { DistIndex element; element.index = distIndices[3].voxelIndex; element.dist = distIndices[3].dist; stack[stackIndex += 1] = element; }
            if (distIndices[4].hit) { DistIndex element; element.index = distIndices[4].voxelIndex; element.dist = distIndices[4].dist; stack[stackIndex += 1] = element; }
            if (distIndices[5].hit) { DistIndex element; element.index = distIndices[5].voxelIndex; element.dist = distIndices[5].dist; stack[stackIndex += 1] = element; }
            if (distIndices[6].hit) { DistIndex element; element.index = distIndices[6].voxelIndex; element.dist = distIndices[6].dist; stack[stackIndex += 1] = element; }
            if (distIndices[7].hit) { DistIndex element; element.index = distIndices[7].voxelIndex; element.dist = distIndices[7].dist; stack[stackIndex += 1] = element; }
            





            //if (h0) { stack[stackIndex += 1] = voxel.k0; }
            //if (h1) { stack[stackIndex += 1] = voxel.k1; }
            //if (h2) { stack[stackIndex += 1] = voxel.k2; }
            //if (h3) { stack[stackIndex += 1] = voxel.k3; }
            //if (h4) { stack[stackIndex += 1] = voxel.k4; }
            //if (h5) { stack[stackIndex += 1] = voxel.k5; }
            //if (h6) { stack[stackIndex += 1] = voxel.k6; }
            //if (h7) { stack[stackIndex += 1] = voxel.k7; }










            //bool h0 = HitAABB3(ray, AABB(vec3(k0.minX, k0.minY, k0.minZ), vec3(k0.maxX, k0.maxY, k0.maxZ)), distances[0]);
            //bool h1 = HitAABB3(ray, AABB(vec3(k1.minX, k1.minY, k1.minZ), vec3(k1.maxX, k1.maxY, k1.maxZ)), distances[1]);
            //bool h2 = HitAABB3(ray, AABB(vec3(k2.minX, k2.minY, k2.minZ), vec3(k2.maxX, k2.maxY, k2.maxZ)), distances[2]);
            //bool h3 = HitAABB3(ray, AABB(vec3(k3.minX, k3.minY, k3.minZ), vec3(k3.maxX, k3.maxY, k3.maxZ)), distances[3]);
            //bool h4 = HitAABB3(ray, AABB(vec3(k4.minX, k4.minY, k4.minZ), vec3(k4.maxX, k4.maxY, k4.maxZ)), distances[4]);
            //bool h5 = HitAABB3(ray, AABB(vec3(k5.minX, k5.minY, k5.minZ), vec3(k5.maxX, k5.maxY, k5.maxZ)), distances[5]);
            //bool h6 = HitAABB3(ray, AABB(vec3(k6.minX, k6.minY, k6.minZ), vec3(k6.maxX, k6.maxY, k6.maxZ)), distances[6]);
            //bool h7 = HitAABB3(ray, AABB(vec3(k7.minX, k7.minY, k7.minZ), vec3(k7.maxX, k7.maxY, k7.maxZ)), distances[7]);


            /*
            Voxel k0 = voxels[voxel.k0];
            Voxel k1 = voxels[voxel.k1];
            Voxel k2 = voxels[voxel.k2];
            Voxel k3 = voxels[voxel.k3];
            Voxel k4 = voxels[voxel.k4];
            Voxel k5 = voxels[voxel.k5];
            Voxel k6 = voxels[voxel.k6];
            Voxel k7 = voxels[voxel.k7];

            float distances[8];

            bool a0 = HitAABB(ray, AABB(vec3(k0.minX, k0.minY, k0.minZ), vec3(k0.maxX, k0.maxY, k0.maxZ)), distances[0]);
            bool a1 = HitAABB(ray, AABB(vec3(k1.minX, k1.minY, k1.minZ), vec3(k1.maxX, k1.maxY, k1.maxZ)), distances[1]);
            bool a2 = HitAABB(ray, AABB(vec3(k2.minX, k2.minY, k2.minZ), vec3(k2.maxX, k2.maxY, k2.maxZ)), distances[2]);
            bool a3 = HitAABB(ray, AABB(vec3(k3.minX, k3.minY, k3.minZ), vec3(k3.maxX, k3.maxY, k3.maxZ)), distances[3]);
            bool a4 = HitAABB(ray, AABB(vec3(k4.minX, k4.minY, k4.minZ), vec3(k4.maxX, k4.maxY, k4.maxZ)), distances[4]);
            bool a5 = HitAABB(ray, AABB(vec3(k5.minX, k5.minY, k5.minZ), vec3(k5.maxX, k5.maxY, k5.maxZ)), distances[5]);
            bool a6 = HitAABB(ray, AABB(vec3(k6.minX, k6.minY, k6.minZ), vec3(k6.maxX, k6.maxY, k6.maxZ)), distances[6]);
            bool a7 = HitAABB(ray, AABB(vec3(k7.minX, k7.minY, k7.minZ), vec3(k7.maxX, k7.maxY, k7.maxZ)), distances[7]);

            if (a0 && a1 && a2 && a3 && a4 && a5 && a6 && a7) {
                return true;
            }
            */

            /*
            // Hit every kid, record the distance
            DistIndex distances[8];

            distances[0].index = 0;
            distances[1].index = 1;
            distances[2].index = 2;
            distances[3].index = 3;
            distances[4].index = 4;
            distances[5].index = 5;
            distances[6].index = 6;
            distances[7].index = 7;

            bool hits[8];

            Voxel k0 = voxels[voxel.k0]; hits[0] = HitAABB(ray, AABB(vec3(k0.minX, k0.minY, k0.minZ), vec3(k0.maxX, k0.maxY, k0.maxZ)), distances[0].dist);
            Voxel k1 = voxels[voxel.k1]; hits[1] = HitAABB(ray, AABB(vec3(k1.minX, k1.minY, k1.minZ), vec3(k1.maxX, k1.maxY, k1.maxZ)), distances[1].dist);
            Voxel k2 = voxels[voxel.k2]; hits[2] = HitAABB(ray, AABB(vec3(k2.minX, k2.minY, k2.minZ), vec3(k2.maxX, k2.maxY, k2.maxZ)), distances[2].dist);
            Voxel k3 = voxels[voxel.k3]; hits[3] = HitAABB(ray, AABB(vec3(k3.minX, k3.minY, k3.minZ), vec3(k3.maxX, k3.maxY, k3.maxZ)), distances[3].dist);            
            Voxel k4 = voxels[voxel.k4]; hits[4] = HitAABB(ray, AABB(vec3(k4.minX, k4.minY, k4.minZ), vec3(k4.maxX, k4.maxY, k4.maxZ)), distances[4].dist);
            Voxel k5 = voxels[voxel.k5]; hits[5] = HitAABB(ray, AABB(vec3(k5.minX, k5.minY, k5.minZ), vec3(k5.maxX, k5.maxY, k5.maxZ)), distances[5].dist);
            Voxel k6 = voxels[voxel.k6]; hits[6] = HitAABB(ray, AABB(vec3(k6.minX, k6.minY, k6.minZ), vec3(k6.maxX, k6.maxY, k6.maxZ)), distances[6].dist);            
            Voxel k7 = voxels[voxel.k7]; hits[7] = HitAABB(ray, AABB(vec3(k7.minX, k7.minY, k7.minZ), vec3(k7.maxX, k7.maxY, k7.maxZ)), distances[7].dist);

            for (int i = 0; i < 8; ++i) {
                if (hits[i] && distances[i].dist > hitInfo.closestDistance) {
                    // Too far, remove it
                    distances[i].index = -1;
                    distances[i].dist = MAX_FLOAT;
                }
            }
            
            // Sort distances
            int n = 8;
            
            for (int i = 0; i < n - 1; i++) {
                bool swapped = false;

                for (int j = 0; j < n - i - 1; j++) {
                    if (distances[j].dist > distances[j + 1].dist) {
                        DistIndex tmp = distances[j];
                        distances[j] = distances[j + 1];
                        distances[j + 1] = tmp;

                        swapped = true;
                    }
                }
      
                // If no elements were swapped, then the array is sorted
                if (!swapped)
                    break;
            }

            for (int i = 0; i < 8; ++i) {
                if (distances[i].index == -1) {
                    break;
                }

                switch (distances[i].index) {
                    case 0: stack[stackIndex += 1] = voxel.k0; break;
                    case 1: stack[stackIndex += 1] = voxel.k1; break;
                    case 2: stack[stackIndex += 1] = voxel.k2; break;
                    case 3: stack[stackIndex += 1] = voxel.k3; break;
                    case 4: stack[stackIndex += 1] = voxel.k4; break;
                    case 5: stack[stackIndex += 1] = voxel.k5; break;
                    case 6: stack[stackIndex += 1] = voxel.k6; break;
                    case 7: stack[stackIndex += 1] = voxel.k7; break;
                }
            }
            */



        }
        else { // No kids, we should shoot a ray at it
            if (voxel.shouldDraw) {
                HitInfo backupHitInfo = hitInfo;
                if (HitAABB2(ray, AABB(vec3(voxel.minX, voxel.minY, voxel.minZ), vec3(voxel.maxX, voxel.maxY, voxel.maxZ)), backupHitInfo)) {
                    if (backupHitInfo.closestDistance < hitInfo.closestDistance) {
                        hitInfo = backupHitInfo;
                        hitSomething = true;
                    }
                }
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

bool IsInterior(float alpha, float beta) {
    return alpha > 0 && beta > 0 && alpha + beta < 1;
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

bool HitAABB2(Ray ray, AABB bbox, inout HitInfo hitInfo) {
#ifdef STATS
    ++stats.bboxChecks;
#endif

    vec3 t0Temp = (bbox.minBound - ray.origin) * ray.inverseDirection;
    vec3 t1Temp = (bbox.maxBound - ray.origin) * ray.inverseDirection;

    vec3 t0 = min(t0Temp, t1Temp);
    vec3 t1 = max(t0Temp, t1Temp);

    float tNear = max(t0.x, max(t0.y, t0.z));
    float tFar = min(t1.x, min(t1.y, t1.z));

    if (tFar < tNear || tFar <= 0 || tNear <= MIN_RAY_DISTANCE) {
        return false; // No hit
    }

    hitInfo.closestDistance = tNear;
    hitInfo.hitPosition = ray.origin + ray.direction * tNear;

    // Determine which axis the hit occurred on by comparing the components of tNear
    vec3 hitNormal;
    if (tNear == t0.x) {
        hitNormal = vec3(ray.inverseDirection.x > 0.0 ? -1.0 : 1.0, 0.0, 0.0);
    } 
    else if (tNear == t0.y) {
        hitNormal = vec3(0.0, ray.inverseDirection.y > 0.0 ? -1.0 : 1.0, 0.0);
    } 
    else if (tNear == t0.z) {
        hitNormal = vec3(0.0, 0.0, ray.inverseDirection.z > 0.0 ? -1.0 : 1.0);
    }

    hitInfo.normal = getFaceNormal(ray, hitNormal);

    hitInfo.hitObjectIndex = 0;

    return true;
}

bool HitAABB3(Ray ray, AABB bbox, inout float dist) {
#ifdef STATS
    ++stats.bboxChecks;
#endif

    vec3 t0Temp = (bbox.minBound - ray.origin) * ray.inverseDirection;
    vec3 t1Temp = (bbox.maxBound - ray.origin) * ray.inverseDirection;

    vec3 t0 = min(t0Temp, t1Temp);
    vec3 t1 = max(t0Temp, t1Temp);

    float tNear = max(t0.x, max(t0.y, t0.z));
    float tFar = min(t1.x, min(t1.y, t1.z));

    dist = tNear;

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
