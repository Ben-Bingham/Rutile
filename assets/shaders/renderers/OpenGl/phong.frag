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
vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir);

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
    
    if (haveDirectionalLight) {
        result += directionalLightAddition(directionalLight, norm, viewDir);
    }

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

vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDirection = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

    // Specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), phong.shininess);

    vec3 ambient = light.ambient * phong.ambient;
    vec3 diffuse = light.diffuse * diff * phong.diffuse;
    vec3 specular = light.specular * spec * phong.specular;

    //diffuse *= (1.0 - shadow);
    //specular *= (1.0 - shadow);

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