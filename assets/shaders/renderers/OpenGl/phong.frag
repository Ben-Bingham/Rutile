#version 430 core

struct Phong {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float farPlane;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 outFragColor;

uniform Phong phong;
uniform vec3 cameraPosition;

in vec3 normal;
in vec3 fragPosition;

const int MAX_LIGHTS = 4;

uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightCount;

uniform DirectionalLight directionalLight;
uniform bool haveDirectionalLight;

vec3 pointLightAddition      (PointLight light,       vec3 normal, vec3 viewDir, float shadow);
vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow);

// Omnidirectional Shadow Maps
float calculateOmnidirectionalShadow(int pointLightIndex, vec3 fragPosition);

uniform bool omnidirectionalShadowMaps;

uniform float omnidirectionalShadowMapBias;

uniform int omnidirectionalShadowMapPCFMode;

uniform int omnidirectionalShadowMapSampleCount;

uniform int omnidirectionalShadowMapDiskRadiusMode;
uniform float omnidirectionalShadowMapDiskRadius;

// Arrays of sampler cubes were causing problems
uniform samplerCube pointLightCubeMap0;
uniform samplerCube pointLightCubeMap1;
uniform samplerCube pointLightCubeMap2;
uniform samplerCube pointLightCubeMap3;

// Directional Shadows
float calculateDirectionalShadow();

uniform bool directionalShadows;

uniform mat4 view;

// Cascading Shadow maps
uniform sampler2DArray cascadingShadowMap;

uniform int cascadeCount;
uniform float cascadeFrustumPlanes[11];

uniform mat4 lightSpaceMatrices[10];

uniform float farPlane;

//vec3 spotLightAddition       (SpotLight light,        vec3 normal, vec3 viewDir, float shadow);

/*
// Omnidirectional Shadow Maps
uniform samplerCube omnidirectionalShadowMaps[MAX_LIGHTS];

// TODO make this an array
uniform float farPlane;

uniform float omnidirectionalShadowMapBias;

uniform int omnidirectionalShadowMapPCFMode;

uniform float omnidirectionalShadowMapPCFSamples;

uniform int omnidirectionalShadowMapDiskRadiusMode;

uniform float omnidirectionalShadowMapDiskRadius;

// Directional Shadow Maps
uniform int shadowMapMode;

uniform sampler2D shadowMap;

in vec4 fragPositionInLightSpace;

uniform int shadowMapBiasMode;

uniform float shadowMapBias;

uniform float dynamicShadowMapBiasMin;
uniform float dynamicShadowMapBiasMax;

uniform int shadowMapPcfMode;

*/

//float shadowCalculationForOmnidirectionalShadowMaps(vec3 fragPosition);

float LinearToGamma(float component) {
    if (component > 0.0) {
        return sqrt(component);
    }
    return 0.0;
}

vec3 LinearToGamma(vec3 color) {
    return vec3(LinearToGamma(color.r), LinearToGamma(color.g), LinearToGamma(color.b));
}

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    
    for (int i = 0; i < pointLightCount; ++i) {
        float shadow = 0.0;
        if (omnidirectionalShadowMaps) {
            shadow = calculateOmnidirectionalShadow(i, fragPosition);
        }
        result += pointLightAddition(pointLights[i], norm, viewDir, shadow);
    }
    
    float shadow = calculateDirectionalShadow();
    if (haveDirectionalLight) {
        float shadow = 0.0;
        if (directionalShadows) {
            shadow = calculateDirectionalShadow();
        }
    
        result += directionalLightAddition(directionalLight, norm, viewDir, shadow);
    }
    
    result = LinearToGamma(result);

    outFragColor = vec4(result, 1.0);
}

vec3 pointLightAddition(PointLight light, vec3 normal, vec3 viewDir, float shadow) {
    //float shadow = shadowCalculationForOmnidirectionalShadowMaps(fragPosition);
    vec3 lightDirection = normalize(light.position - fragPosition);

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

    // Specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), phong.shininess);

    // Attenuation
    float lightDistance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

    vec3 ambient = light.ambient * phong.ambient;
    vec3 diffuse = light.diffuse * diff * phong.diffuse;
    vec3 specular = light.specular * spec * phong.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return (ambient + diffuse + specular);
}

vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDirection = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

    // Specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), phong.shininess);

    vec3 ambient = light.ambient * phong.ambient;
    vec3 diffuse = light.diffuse * diff * phong.diffuse;
    vec3 specular = light.specular * spec * phong.specular;

    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return (ambient + diffuse + specular);
}

float calculateOmnidirectionalShadow(int pointLightIndex, vec3 fragPosition) {
    float shadow = 0.0;
    vec3 fragmentToLight = fragPosition - pointLights[pointLightIndex].position;

    float closestDepth = 0.0;

    switch(pointLightIndex) {
        case 0:
            closestDepth = texture(pointLightCubeMap0, fragmentToLight).r;
            break;
        case 1:
            closestDepth = texture(pointLightCubeMap1, fragmentToLight).r;
            break;
        case 2:
            closestDepth = texture(pointLightCubeMap2, fragmentToLight).r;
            break;
        case 3:
            closestDepth = texture(pointLightCubeMap3, fragmentToLight).r;
            break;
    }

    closestDepth *= pointLights[pointLightIndex].farPlane;

    float currentDepth = length(fragmentToLight);

    float bias    = omnidirectionalShadowMapBias; 

    if (omnidirectionalShadowMapPCFMode == 0) {
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    } else if (omnidirectionalShadowMapPCFMode == 1) {
        float samples = float(omnidirectionalShadowMapSampleCount);
        float offset  = 0.1;
        for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
            for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
                for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                    float closestDepth = 0.0;
                    switch(pointLightIndex) {
                        case 0:
                            closestDepth = texture(pointLightCubeMap0, fragmentToLight + vec3(x, y, z)).r;
                            break;
                        case 1:
                            closestDepth = texture(pointLightCubeMap1, fragmentToLight + vec3(x, y, z)).r;
                            break;
                        case 2:
                            closestDepth = texture(pointLightCubeMap2, fragmentToLight + vec3(x, y, z)).r;
                            break;
                        case 3:
                            closestDepth = texture(pointLightCubeMap3, fragmentToLight + vec3(x, y, z)).r;
                            break;
                    }
                    closestDepth *= pointLights[pointLightIndex].farPlane;
                    if(currentDepth - bias > closestDepth) {
                        shadow += 1.0;
                    }
                }
            }
        }

        shadow /= (samples * samples * samples);

    } else if (omnidirectionalShadowMapPCFMode == 2) {
        vec3 sampleOffsetDirections[20] = vec3[](
            vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
            vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
            vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
            vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
            vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
        );

        shadow = 1.0;

        int samples = 20; // The size of sampleOffsetDirections
        float viewDistance = length(cameraPosition - fragPosition);
        float diskRadius = omnidirectionalShadowMapDiskRadius;
        if (omnidirectionalShadowMapDiskRadiusMode == 1) {
            diskRadius = (1.0 + (viewDistance / pointLights[pointLightIndex].farPlane)) / 25.0;
        }
            
        for(int j = 0; j < samples; ++j) {
            float closestDepth = 0.0;
            switch(pointLightIndex) {
                case 0:
                    closestDepth = texture(pointLightCubeMap0, fragmentToLight + sampleOffsetDirections[j] * diskRadius).r;
                    break;
                case 1:
                    closestDepth = texture(pointLightCubeMap1, fragmentToLight + sampleOffsetDirections[j] * diskRadius).r;
                    break;
                case 2:
                    closestDepth = texture(pointLightCubeMap2, fragmentToLight + sampleOffsetDirections[j] * diskRadius).r;
                    break;
                case 3:
                    closestDepth = texture(pointLightCubeMap3, fragmentToLight + sampleOffsetDirections[j] * diskRadius).r;
                    break;
            }
            closestDepth *= pointLights[pointLightIndex].farPlane;
            if(currentDepth - bias > closestDepth) { 
                shadow += 1.0;
            }
        }
        shadow /= float(samples); 
    }

    return shadow;
}

float calculateDirectionalShadow() {
    //vec4 fragPositionViewSpace = view * vec4(fragPosition, 1.0);
    //float fragmentDepth = abs(fragPositionViewSpace.z);
    //
    //outFragColor = vec4((view * vec4(fragPosition.xyz, 1.0)).xyz, 1.0);
    //outFragColor = vec4(fragmentDepth, fragmentDepth, fragmentDepth, 1.0);
    //
    //
    //int layer = -1;
    //for (int i = 0; i < cascadeCount; ++i) {
    //    if (fragmentDepth < cascadeFrustumPlanes[i + 1]) {
    //        layer = i;
    //        break;
    //    }
    //}
    //if (layer == -1) {
    //    layer = cascadeCount;
    //}
    //
    //float l = float(layer) / float(cascadeCount);
    //
    ////outFragColor = vec4(l, l, l, 1.0);
    //
    //vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosition, 1.0);
    //
    ////outFragColor = vec4(fragPosLightSpace.xyz, 1.0);
    //
    //vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //projCoords = projCoords * 0.5 + 0.5;
    //
    ////outFragColor = vec4(projCoords.xyz, 1.0);
    //
    //float currentDepth = projCoords.z;
    //
    ////outFragColor = vec4(currentDepth, currentDepth, currentDepth, 1.0);
    //
    //float depth = texture(cascadingShadowMap, vec3(projCoords.xy, layer)).r;
    //
    //outFragColor = vec4(depth, depth, depth, 1.0);
    //
    //float bias = 0.005;
    //float shadow = (currentDepth - bias) > depth ? 1.0 : 0.0;
    ////if (layer == 0) {
    ////    float shadow = (currentDepth - bias) > depth ? 1.0 : 0.0;
    //outFragColor = vec4(shadow, shadow, shadow, 1.0);
    //
    //return shadow;
























    vec4 fragPositionViewSpace = view * vec4(fragPosition, 1.0);
    float fragmentDepth = abs(fragPositionViewSpace.z);
    
    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i) {
        if (fragmentDepth < cascadeFrustumPlanes[i + 1]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = cascadeCount - 1;
    }
    
    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosition, 1.0);
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float currentDepth = projCoords.z;
    //if (currentDepth > 1.0) {
    //    return 0.0;
    //}
    
    //vec3 norm = normalize(normal);
    //float bias = max(0.05 * (1.0 - dot(norm, directionalLight.direction)), 0.005);
    //if (layer == cascadeCount) {
    //    bias *= 1.0 / (farPlane * 0.5);
    //}
    //else {
    //    bias *= 1.0 / (cascadeFrustumPlanes[layer] * 0.5);
    //}
    
    //if (projCoords.z > 1.0) {
    //    return 0.0;
    //}
    
    float depth = texture(cascadingShadowMap, vec3(projCoords.xy, layer)).r;
    
    outFragColor = vec4(depth, depth, depth, 1.0);
    
    //return depth;

    float bias = 0.005f;
    //return 1.0;

    //return float(layer) / float(cascadeCount);
    
    return (currentDepth - bias) > depth ? 1.0 : 0.0;
    
    
    //float shadow = 0.0;
    //vec2 texelSize = 1.0 / vec2(textureSize(cascadingShadowMap, 0));
    //for(int x = -1; x <= 1; ++x)
    //{
    //    for(int y = -1; y <= 1; ++y)
    //    {
    //        float pcfDepth = texture(cascadingShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
    //        shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
    //    }
    //}
    //shadow /= 9.0;
    //
    //return shadow;
}