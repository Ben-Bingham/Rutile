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

int HitChunk(Ray ray, vec3 offset, inout float closestHit) {
    int density = 8;

    float w = 1.0 / float(density);

    int closestIndex = -1;

    for (int x = 0; x < density; ++x) {
        for (int y = 0; y < density; ++y) {
            for (int z = 0; z < density; ++z) {
                vec3 m = vec3(x * w, y * w, z * w) + offset;
                vec3 M = vec3(w + x * w, w + y * w, w + z * w) + offset;

                AABB bbox = AABB(m, M);

                float dist;

                if (HitAABB(ray, bbox, dist) && dist < closestHit) {
                    closestHit = dist;
                    closestIndex = x + y + z;
                }
            }
        }
    }

    return closestIndex;
}

uniform int octTreeX;
uniform int octTreeY;
uniform int octTreeZ;

uniform int octTreeXL2;
uniform int octTreeYL2;
uniform int octTreeZL2;

struct Octree {
    bool a;
    bool b;
    bool c;
    bool d;
    bool e;
    bool f;
    bool g;
    bool h;

    int index;
};

int SetBit(inout int value, int n) {
    return value |= (1 << n);
}

bool GetBit(int value, int n) {
    return bool((value >> n) & 1);
}

int GetOctreeIndex(int octree) {
    int index = octree << 8;
    index = index >> 8;

    return index;
}

// Returns true if the octree has at least 1 child
bool OctreeHasKids(int octree) {
    return bool(octree >> 24);
}

const int OCT_CHILD_0 = 24;
const int OCT_CHILD_1 = 25;
const int OCT_CHILD_2 = 26;
const int OCT_CHILD_3 = 27;

const int OCT_CHILD_4 = 28;
const int OCT_CHILD_5 = 29;
const int OCT_CHILD_6 = 30;
const int OCT_CHILD_7 = 31;

uniform int octTreeNoKids;

uniform int octree;
uniform int octreeChild;

uniform int child1;
uniform int child2;
uniform int child3;
uniform int child4;
uniform int child5;
uniform int child6;
uniform int child7;
uniform int child8;

uniform float startingWidth;

// OLD Returns the number of the block it hit, or -1 if it did not hit the block OLD

struct Ret {
    int mask;

    float dist0;
    float dist1;
    float dist2;
    float dist3;
    float dist4;
    float dist5;
    float dist6;
    float dist7;
};

// Returns a mask where all bits set represent a child hit
int HitOctree(Ray ray, inout float currentOctantWidth, inout vec3 octreeCenter, int targetChild, bool root, int octree) {
    float dist = MAX_FLOAT;
    if (!HitAABB(ray, AABB(octreeCenter - currentOctantWidth, octreeCenter + currentOctantWidth), dist)) {
        // We did not even hit the octant that this octree is inside of
        return -1;
    }

    int closestHitIndex = -1;
    float closestHit = MAX_FLOAT;

    if (!root) {
        currentOctantWidth /= 2.0;

        switch (targetChild) {
        case 0:
            octreeCenter -= currentOctantWidth;
            break;
        case 1:
            octreeCenter += vec3(currentOctantWidth, -currentOctantWidth, -currentOctantWidth);
            break;
        case 2:
            octreeCenter += vec3(-currentOctantWidth, -currentOctantWidth, currentOctantWidth);
            break;
        case 3:
            octreeCenter += vec3(currentOctantWidth, -currentOctantWidth, currentOctantWidth);
            break;
        case 4:
            octreeCenter += vec3(-currentOctantWidth, currentOctantWidth, -currentOctantWidth);
            break;
        case 5:
            octreeCenter += vec3(currentOctantWidth, currentOctantWidth, -currentOctantWidth);
            break;
        case 6:
            octreeCenter += vec3(-currentOctantWidth, currentOctantWidth, currentOctantWidth);
            break;
        case 7:
            octreeCenter += currentOctantWidth;
            break;
        };
    }

    float dist0 = MAX_FLOAT;
    float dist1 = MAX_FLOAT;
    float dist2 = MAX_FLOAT;
    float dist3 = MAX_FLOAT;
    float dist4 = MAX_FLOAT;
    float dist5 = MAX_FLOAT;
    float dist6 = MAX_FLOAT;
    float dist7 = MAX_FLOAT;

    AABB bbox0 = AABB(octreeCenter - currentOctantWidth, octreeCenter);
    AABB bbox1 = AABB(octreeCenter - vec3(0.0, currentOctantWidth, currentOctantWidth), octreeCenter + vec3(currentOctantWidth, 0.0, 0.0));
    AABB bbox2 = AABB(octreeCenter - vec3(currentOctantWidth, currentOctantWidth, 0.0), octreeCenter + vec3(0.0, 0.0, currentOctantWidth));
    AABB bbox3 = AABB(octreeCenter - vec3(0.0, currentOctantWidth, 0.0), octreeCenter + vec3(currentOctantWidth, 0.0, currentOctantWidth));
    AABB bbox4 = AABB(octreeCenter - vec3(currentOctantWidth, 0.0, currentOctantWidth), octreeCenter + vec3(0.0, currentOctantWidth, 0.0));
    AABB bbox5 = AABB(octreeCenter - vec3(0.0, 0.0, currentOctantWidth), octreeCenter + vec3(currentOctantWidth, currentOctantWidth, 0.0));
    AABB bbox6 = AABB(octreeCenter - vec3(currentOctantWidth, 0.0, 0.0), octreeCenter + vec3(0.0, currentOctantWidth, currentOctantWidth));
    AABB bbox7 = AABB(octreeCenter, octreeCenter + currentOctantWidth);

    if (GetBit(octree, OCT_CHILD_0) && HitAABB(ray, bbox0, dist0)) {
        //ret = SetBit(ret, OCT_CHILD_0);

        if (dist0 < closestHit) {
            closestHit = dist0;
            closestHitIndex = 0;
        }
    }

    if (GetBit(octree, OCT_CHILD_1) && HitAABB(ray, bbox1, dist1)) {     
        //ret = SetBit(ret, OCT_CHILD_1);

        if (dist1 < closestHit) {
            closestHit = dist1;
            closestHitIndex = 1;
        }
    }
        
    if (GetBit(octree, OCT_CHILD_2) && HitAABB(ray, bbox2, dist2)) {
        //ret = SetBit(ret, OCT_CHILD_2);

        if (dist2 < closestHit) {
            closestHit = dist2;
            closestHitIndex = 2;
        }
    }
        
    if (GetBit(octree, OCT_CHILD_3) && HitAABB(ray, bbox3, dist3)) {        
        //ret = SetBit(ret, OCT_CHILD_3);

        if (dist3 < closestHit) {
            closestHit = dist3;
            closestHitIndex = 3;
        }
    }
        
    if (GetBit(octree, OCT_CHILD_4) && HitAABB(ray, bbox4, dist4)) {     
        //ret = SetBit(ret, OCT_CHILD_4);

        if (dist4 < closestHit) {
            closestHit = dist4;
            closestHitIndex = 4;
        }
    }
        
    if (GetBit(octree, OCT_CHILD_5) && HitAABB(ray, bbox5, dist5)) {   
        //ret = SetBit(ret, OCT_CHILD_5);

        if (dist5 < closestHit) {
            closestHit = dist5;
            closestHitIndex = 5;
        }
    }
        
    if (GetBit(octree, OCT_CHILD_6) && HitAABB(ray, bbox6, dist6)) {       
        //ret = SetBit(ret, OCT_CHILD_6);

        if (dist6 < closestHit) {
            closestHit = dist6;
            closestHitIndex = 6;
        }
    }
        
    if (GetBit(octree, OCT_CHILD_7) && HitAABB(ray, bbox7, dist7)) {     
        //ret = SetBit(ret, OCT_CHILD_7);

        if (dist7 < closestHit) {
            closestHit = dist7;
            closestHitIndex = 7;
        }
    }

    return closestHitIndex;
    //return ret;
}

uniform int rootOctreeChildren[8];

vec3 FireRayIntoScene(Ray r) {
    vec3 color = vec3(0.0, 0.0, 0.0);
    vec3 throughput = vec3(1.0, 1.0, 1.0);






    int stack[32];
    int stackIndex = 0;

    stack[stackIndex] = 0;

    while (stackIndex >= 0) {
        int tree;

        // TODO this switch is temporary before i swap in an array
        // DO HARDCODED ARRAY TESTING BEFORE SSBO TeSTInG
        switch (stack[stackIndex]) {
        case 0: // Root
            tree = octree;
            break;
        };
        --stackIndex;

        /*
        if (dont hit octree) { 
            modify octree center and width
            continue 
        }

        for (child : octree children) {
            if (octree has ith child) {
                if (we hit octress ith child) {
                    Add the child to the stack
                }
            }
        }

        modify octree center and width
        */
    }



























    bool hitSomething = false;

    float maxOctantWidth = startingWidth;

    // root, is the root, and so we know its size:
    vec3 octreeCenter = vec3(0.0, 0.0, 0.0);
    //vec3 currentOctantWidth3 = vec3(maxOctantWidth);
    float currentOctantWidth = maxOctantWidth;

    //vec3 octTreeMin = vec3(-maxOctantWidth);
    //vec3 octTreeMax = vec3(maxOctantWidth);

    // We need to do a special hit function here because its the root node. All other octrees are only even considerd if we hit there surounding bbox,
    // But the root node needs to be checked manualy first
    float closestHit = MAX_FLOAT;
    int closestHitIndex = -1;

    vec3 finalColor = vec3(0.0);

    float dist;
    if (HitAABB(r, AABB(octreeCenter - currentOctantWidth, octreeCenter + currentOctantWidth), dist)) {
        finalColor = vec3(1.0);
    }

    closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, 0, true, octree);
    // We now have the index of the closest child of this octree. If this index is equal to any of the bits toggled in the first 8 bits of an octree
    // than we want to continue, and check the next octree (usualy this would be done with the index system). If instead the hit octant dosent have
    // the bit for the hit child set, than we check if any of the child bits are set. If none of the child bits are set, and the index is 0, the octree
    // is empty, and we can break out. But if there is an index, than we have a hit, and the index is a material index.




    if (closestHitIndex != -1 && GetBit(octree, OCT_CHILD_0 + closestHitIndex)) {
    //if () {
        //int childOctree = 0;
        //childOctree = SetBit(childOctree, OCT_CHILD_0);
        //closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, closestHitIndex, false, childOctree);
        //
        //if (closestHitIndex == 0) {
        //    return vec3(0.0, 1.0, 0.0);
        //} else {
        //    return vec3(0.0, 0.0, 1.0);
        //}
        return vec3(0.0, 1.0, 0.0);

    } else {
        return vec3(1.0, 0.0, 0.0);
    }











    if (closestHitIndex == child1) {
        // Hit the child
        finalColor = vec3(1.0, 0.0, 0.0);

        closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child1, false, 0);

        if (closestHitIndex == child2) {
            // Hit the child
            finalColor = vec3(0.0, 1.0, 0.0);

            closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child2, false, 0);

            if (closestHitIndex == child3) {
                // Hit the child
                finalColor = vec3(0.0, 0.0, 1.0);

                closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child3, false, 0);

                if (closestHitIndex == child4) {
                    // Hit the child
                    finalColor = vec3(1.0, 1.0, 0.0);

                    closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child4, false, 0);

                    if (closestHitIndex == child5) {
                        // Hit the child
                        finalColor = vec3(0.0, 1.0, 1.0);

                        closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child5, false, 0);

                        if (closestHitIndex == child6) {
                            // Hit the child
                            finalColor = vec3(1.0, 0.0, 1.0);
                        
                            closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child6, false, 0);
                        
                            if (closestHitIndex == child7) {
                                // Hit the child
                                finalColor = vec3(1.0, 0.5, 0.5);
                        
                                closestHitIndex = HitOctree(r, currentOctantWidth, octreeCenter, child7, false, 0);
                        
                                if (closestHitIndex == child8) {
                                    // Hit the child
                                    finalColor = vec3(0.5, 0.5, 1.0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }   
    //else {
        // Did not hit anything
        //finalColor = vec3(0.0, 0.0, 0.0);
    //}
    //} 

    return finalColor;


    //return vec3(0.0, 0.0, 0.0);

    const int rootNodeIndex = 0;




    /*
    //float dist;
    //if (HitAABB(r, AABB(octTreeMin, octTreeMax), dist)) {
    while (stackIndex >= 0) {
        int index = stack[stackIndex];
        --stackIndex;

        int octree = octrees[index];

        if (!OctreeHasKids(octree)) {
            // The octree has NO children

            if (GetOctreeIndex(octree) == 0) {
                // Octree is empty
                hitSomething = false;
                // Break out of loop
            } else {
                // Octree is solid, index points to the material
                hitSomething = true;
                // Break out of loop
            }
        } 
        else {
            // Octree has at least one child
            // We need the sizes of the children octants themselves, we then need to hit test them, and if we hit them, we add them to the stack
            vec3 minA = vec3(-maxOctantWidth);
            vec3 maxA = vec3(0.0);

            vec3 minB = vec3(0.0, -maxOctantWidth, -maxOctantWidth);
            vec3 maxB = vec3(maxOctantWidth, 0.0, 0.0);

            vec3 minC = vec3(-maxOctantWidth, -maxOctantWidth, 0.0);
            vec3 maxC = vec3(0.0, 0.0, maxOctantWidth);

            vec3 minD = vec3(0.0, -maxOctantWidth, 0.0);
            vec3 maxD = vec3(maxOctantWidth, 0.0, maxOctantWidth);

            vec3 minE = vec3(-maxOctantWidth, 0.0, -maxOctantWidth);
            vec3 maxE = vec3(0.0, maxOctantWidth, 0.0);

            vec3 minF = vec3(0.0, 0.0, -maxOctantWidth);
            vec3 maxF = vec3(maxOctantWidth, maxOctantWidth, 0.0);

            vec3 minG = vec3(-maxOctantWidth, 0.0, 0.0);
            vec3 maxG = vec3(0.0, maxOctantWidth, maxOctantWidth);

            vec3 minH = vec3(0.0);
            vec3 maxH = vec3(maxOctantWidth);

            vec3 chosenMin;
            vec3 chosenMax;

            int indexOffset = 0;

            if (GetBit(octree, OCT_CHILD_0)) {
                chosenMin = minA;
                chosenMax = maxA;

                // TODO this chunk adds them all in the ssame order, find out how to add them based on which is closest.
                //++stackIndex;
                //stack[stackIndex] = index + indexOffset;
                //++indexOffset;
            }

            if (GetBit(octree, OCT_CHILD_1)) {
                chosenMin = minB;
                chosenMax = maxB;
            }

            if (GetBit(octree, OCT_CHILD_2)) {
                chosenMin = minC;
                chosenMax = maxC;
            }

            if (GetBit(octree, OCT_CHILD_3)) {
                chosenMin = minD;
                chosenMax = maxD;
            }

            if (GetBit(octree, OCT_CHILD_4)) {
                chosenMin = minE;
                chosenMax = maxE;
            }

            if (GetBit(octree, OCT_CHILD_5)) {
                chosenMin = minF;
                chosenMax = maxF;
            }

            if (GetBit(octree, OCT_CHILD_6)) {
                chosenMin = minG;
                chosenMax = maxG;
            }

            if (GetBit(octree, OCT_CHILD_7)) {
                chosenMin = minH;
                chosenMax = maxH;
            }

            if (HitAABB(r, AABB(chosenMin, chosenMax), dist)) {
                hitSomething = true;
            }
        }
    }
    //}

    if (hitSomething) {
        return vec3(0.0, 1.0, 0.0);
    }

    return vec3(1.0, 0.0, 0.0);



    */
































    /*
    bool hitSomething = false;

    float maxOctantWidth = 1.0;

    // root, is the root, and so we know its size:
    vec3 octTreeMin = vec3(-maxOctantWidth);
    vec3 octTreeMax = vec3(maxOctantWidth);

    // We need to do a special hit function here because its the root node. All other octrees are only even considerd if we hit there surounding bbox,
    // But the root node needs to be checked manualy first
    float dist;
    if (!HitAABB(r, AABB(octTreeMin, octTreeMax), dist)) {
        hitSomething = false;
        return vec3(1.0, 0.0, 0.0);
        // Return from the function
    }

    const int rootNodeIndex = 0;

    // This list will eventually be passed by an SSBO
    int[64] octrees;
    octrees[rootNodeIndex] = octree;

    int[32] stack;
    int stackIndex = 0;

    stack[stackIndex] = rootNodeIndex;

    //float dist;
    //if (HitAABB(r, AABB(octTreeMin, octTreeMax), dist)) {
    while (stackIndex >= 0) {
        int index = stack[stackIndex];
        --stackIndex;

        int octree = octrees[index];

        if (!OctreeHasKids(octree)) {
            // The octree has NO children

            if (GetOctreeIndex(octree) == 0) {
                // Octree is empty
                hitSomething = false;
                // Break out of loop
            } else {
                // Octree is solid, index points to the material
                hitSomething = true;
                // Break out of loop
            }
        } 
        else {
            // Octree has at least one child
            // We need the sizes of the children octants themselves, we then need to hit test them, and if we hit them, we add them to the stack
            vec3 minA = vec3(-maxOctantWidth);
            vec3 maxA = vec3(0.0);

            vec3 minB = vec3(0.0, -maxOctantWidth, -maxOctantWidth);
            vec3 maxB = vec3(maxOctantWidth, 0.0, 0.0);

            vec3 minC = vec3(-maxOctantWidth, -maxOctantWidth, 0.0);
            vec3 maxC = vec3(0.0, 0.0, maxOctantWidth);

            vec3 minD = vec3(0.0, -maxOctantWidth, 0.0);
            vec3 maxD = vec3(maxOctantWidth, 0.0, maxOctantWidth);

            vec3 minE = vec3(-maxOctantWidth, 0.0, -maxOctantWidth);
            vec3 maxE = vec3(0.0, maxOctantWidth, 0.0);

            vec3 minF = vec3(0.0, 0.0, -maxOctantWidth);
            vec3 maxF = vec3(maxOctantWidth, maxOctantWidth, 0.0);

            vec3 minG = vec3(-maxOctantWidth, 0.0, 0.0);
            vec3 maxG = vec3(0.0, maxOctantWidth, maxOctantWidth);

            vec3 minH = vec3(0.0);
            vec3 maxH = vec3(maxOctantWidth);

            vec3 chosenMin;
            vec3 chosenMax;

            int indexOffset = 0;

            if (GetBit(octree, OCT_CHILD_0)) {
                chosenMin = minA;
                chosenMax = maxA;

                // TODO this chunk adds them all in the ssame order, find out how to add them based on which is closest.
                //++stackIndex;
                //stack[stackIndex] = index + indexOffset;
                //++indexOffset;
            }

            if (GetBit(octree, OCT_CHILD_1)) {
                chosenMin = minB;
                chosenMax = maxB;
            }

            if (GetBit(octree, OCT_CHILD_2)) {
                chosenMin = minC;
                chosenMax = maxC;
            }

            if (GetBit(octree, OCT_CHILD_3)) {
                chosenMin = minD;
                chosenMax = maxD;
            }

            if (GetBit(octree, OCT_CHILD_4)) {
                chosenMin = minE;
                chosenMax = maxE;
            }

            if (GetBit(octree, OCT_CHILD_5)) {
                chosenMin = minF;
                chosenMax = maxF;
            }

            if (GetBit(octree, OCT_CHILD_6)) {
                chosenMin = minG;
                chosenMax = maxG;
            }

            if (GetBit(octree, OCT_CHILD_7)) {
                chosenMin = minH;
                chosenMax = maxH;
            }

            if (HitAABB(r, AABB(chosenMin, chosenMax), dist)) {
                hitSomething = true;
            }
        }
    }
    //}

    if (hitSomething) {
        return vec3(0.0, 1.0, 0.0);
    }

    return vec3(1.0, 0.0, 0.0);

    */




    //int density = 8;
    //
    //// width
    //float w = 1.0 / float(density);
    //
    //float closestHit = MAX_FLOAT;
    //
    //vec3 offset = vec3(3.0, 0.0, -2.0);
    //int closestIndex = HitChunk(r, offset, closestHit);
    //
    //if (closestIndex == -1) {
    //    vec3 offset2 = vec3(2.0, 0.0, -2.0);
    //    closestIndex = HitChunk(r, offset2, closestHit);
    //}
    //
    //if (closestIndex != -1) {
    //    if (closestIndex % 2 == 0) {
    //        return vec3(0.0, 1.0, 0.0);
    //    } else {
    //        return vec3(0.0, 0.0, 1.0);
    //    }
    //} else {
    //    return vec3(1.0, 0.0, 0.0);
    //}


    //int octTreeX = 0;
    //int octTreeY = 0;
    //int octTreeZ = 0;

    float octtantWidth = 1.0;

    // These are the min and max of the fully negative oct tree octant
    //vec3 octTreeMin = vec3(-octtantWidth);
    //vec3 octTreeMax = vec3(0.0);

    vec3 offset = vec3(octTreeX * octtantWidth, octTreeY * octtantWidth, octTreeZ * octtantWidth);

    float halfOctantWidth = octtantWidth / 2.0;

    offset += vec3(octTreeXL2 * halfOctantWidth, octTreeYL2 * halfOctantWidth, octTreeZL2 * halfOctantWidth);

    //vec3 m = octTreeMin + offset;
    //vec3 M = m + vec3(halfOctantWidth);

    //float dist;

    //if (HitAABB(r, AABB(m, M), dist)) {
    //    return vec3(0.0, 1.0, 0.0);
    //} else {
    //    return vec3(1.0, 0.0, 0.0);
    //}






    Ray ray = r;

    int bounces = 0;
    while (true) {
        if (bounces >= maxBounces) {
            break;
        }

        HitInfo hitInfo;
        if (HitScene(ray, hitInfo)) { // Scatter the ray
            
            return vec3(0.0, 1.0, 0.0);

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

    return vec3(1.0, 0.0, 0.0);

    return color;
}

// Stack concept, and some optimzations taken from:
// https://github.com/SebLague/Ray-Tracing/tree/main

bool HitScene(Ray ray, inout HitInfo hitInfo) {
    hitInfo.closestDistance = MAX_FLOAT;
    bool hitSomething = false;
 
    //AABB aabbs[8] = AABB[8](
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0)),
    //    AABB(vec3(0.0), vec3(1.0))
    //);


    int density = 8;

    // width
    float w = 1.0 / float(density);

    bool hit = false;
    for (int i = 0; i < density; ++i) {
        vec3 m = vec3(0.0 + i * w, 0.0, 0.0);
        vec3 M = vec3(w + i * w, w, w);

        AABB bbox = AABB(m, M);

        float dist;
        hit = HitAABB(ray, bbox, dist);
    }
    
    return hit;

    /*
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
    */

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
