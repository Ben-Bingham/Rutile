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
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const int MAX_LIGHTS = 24;

out vec4 outFragColor;

uniform Phong phong;
uniform vec3 cameraPosition;

uniform PointLight pointLights[MAX_LIGHTS];
uniform int pointLightCount;

uniform DirectionalLight directionalLight;

uniform SpotLight spotLights[MAX_LIGHTS];
uniform int spotLightCount;

in vec3 normal;
in vec3 fragPosition;

vec3 pointLightAddition      (PointLight light,       vec3 normal, vec3 viewDir, float shadow);
vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 spotLightAddition       (SpotLight light,        vec3 normal, vec3 viewDir, float shadow);

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

// Returns 1.0 the fragment is in shadow, and 0.0 when its not in shadow
float shadowCalculationForOneShadowEmitter(vec4 fragPositionInLightSpace);

float shadowCalculationForOmnidirectionalShadowMaps(vec3 fragPosition);

float calculateShadows(vec4 fragPositionInLightSpace, vec3 fragPosition);

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    float shadow = calculateShadows(fragPositionInLightSpace, fragPosition);

    for (int i = 0; i < pointLightCount; ++i) {
        result += pointLightAddition(pointLights[i], norm, viewDir, shadow);
    }

    result += directionalLightAddition(directionalLight, norm, viewDir, shadow);

    for (int i = 0; i < spotLightCount; ++i) {
        result += spotLightAddition(spotLights[i], norm, viewDir, shadow);
    }

    outFragColor = vec4(result, 1.0);
}

vec3 pointLightAddition(PointLight light, vec3 normal, vec3 viewDir, float shadow) {
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

vec3 spotLightAddition(SpotLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDirection = normalize(light.position - fragPosition);

    // Diffuse
    float diff = max(dot(normal, lightDirection), 0.0);

    // Specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDirection), 0.0), phong.shininess);

    // Attenuation
    float lightDistance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

    // Spotlight intensity
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * phong.ambient;
    vec3 diffuse = light.diffuse * diff * phong.diffuse;
    vec3 specular = light.specular * spec * phong.specular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return (ambient + diffuse + specular);
}

float shadowCalculationForOneShadowEmitter(vec4 fragPositionInLightSpace) {
    vec3 projectionCoords = fragPositionInLightSpace.xyz / fragPositionInLightSpace.w;

    float shadow = 0.0;
    if (projectionCoords.z > 1.0) {
        return shadow;
    }    

    projectionCoords = projectionCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projectionCoords.xy).r;

    float currentDepth = projectionCoords.z;

    vec3 lightDirection = normalize(-directionalLight.direction);

    float bias = 0.0;
    if (shadowMapBiasMode == 1) {
        bias = shadowMapBias;
    } else if (shadowMapBiasMode == 2) {
        bias = max(dynamicShadowMapBiasMax * (1.0 - dot(normal, lightDirection)), dynamicShadowMapBiasMin);
    }

    if (shadowMapPcfMode == 0) {
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    } else if (shadowMapPcfMode == 1) {
        shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

        for(int x = -1; x <= 1; ++x) {
            for(int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(shadowMap, projectionCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    }

    return shadow;
}

float shadowCalculationForOmnidirectionalShadowMaps(vec3 fragPosition) {
    float shadow = 0.0;
    for (int i = 0; i < pointLightCount; ++i) {
        vec3 fragmentToLight = fragPosition - pointLights[i].position;
        float closestDepth = texture(omnidirectionalShadowMaps[i], fragmentToLight).r;

        closestDepth *= farPlane;

        float currentDepth = length(fragmentToLight);

        float localShadow  = 0.0;
        float bias    = omnidirectionalShadowMapBias; 

        if (omnidirectionalShadowMapPCFMode == 0) {
            localShadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        } else if (omnidirectionalShadowMapPCFMode == 1) {
            float samples = omnidirectionalShadowMapPCFSamples;
            float offset  = 0.1;
            for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
                for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
                    for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                        float closestDepth = texture(omnidirectionalShadowMaps[i], fragmentToLight + vec3(x, y, z)).r; 
                        closestDepth *= farPlane;   // undo mapping [0;1]
                        if(currentDepth - bias > closestDepth) {
                            localShadow += 1.0;
                        }
                    }
                }
            }

            localShadow /= (samples * samples * samples);

        } else if (omnidirectionalShadowMapPCFMode == 2) {
            vec3 sampleOffsetDirections[20] = vec3[](
               vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
               vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
               vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
               vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
               vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
            );

            int samples = 20;
            float viewDistance = length(cameraPosition - fragPosition);
            float diskRadius = omnidirectionalShadowMapDiskRadius;
            if (omnidirectionalShadowMapDiskRadiusMode == 1) {
                diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;  
            }
            
            for(int i = 0; i < samples; ++i) {
                float closestDepth = texture(omnidirectionalShadowMaps[i], fragmentToLight + sampleOffsetDirections[i] * diskRadius).r;
                closestDepth *= farPlane;   // undo mapping [0;1]
                if(currentDepth - bias > closestDepth) {
                    localShadow += 1.0;
                }
            }
            localShadow /= float(samples); 
        }
        
        shadow += localShadow;
    }
    return min(shadow, 1.0);
}

float calculateShadows(vec4 fragPositionInLightSpace, vec3 fragPosition) {
    float shadow = 0.0;

    if (shadowMapMode == 0) {
        shadow = 0.0;
    } else if (shadowMapMode == 1) {
        shadow = shadowCalculationForOneShadowEmitter(fragPositionInLightSpace);
    } else if (shadowMapMode == 2) {
        // TODO Cascading Shadow Maps
    }

    float omnidirectionalShadow = shadowCalculationForOmnidirectionalShadowMaps(fragPosition);
    
    //return shadow + omnidirectionalShadow;
    return omnidirectionalShadow;
}