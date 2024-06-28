#version 330 core

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

uniform DirectionalLight directionalLights[MAX_LIGHTS];
uniform int directionalLightCount;

uniform SpotLight spotLights[MAX_LIGHTS];
uniform int spotLightCount;

in vec3 normal;
in vec3 fragPosition;

vec3 pointLightAddition      (PointLight light,       vec3 normal, vec3 viewDir, float shadow);
vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 spotLightAddition       (SpotLight light,        vec3 normal, vec3 viewDir, float shadow);

// Shadow Maps
uniform sampler2D shadowMap;

in vec4 fragPositionInLightSpace;

uniform float shadowMapBias;

// Returns 1.0 the fragment is in shadow, and 0.0 when its not in shadow
float shadowCalculation(vec4 fragPositionInLightSpace);

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    float shadow = shadowCalculation(fragPositionInLightSpace);

    for (int i = 0; i < pointLightCount; ++i) {
        result += pointLightAddition(pointLights[i], norm, viewDir, shadow);
    }

    for (int i = 0; i < directionalLightCount; ++i) {
        result += directionalLightAddition(directionalLights[i], norm, viewDir, shadow);
    }

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

float shadowCalculation(vec4 fragPositionInLightSpace) {
    vec3 projectionCoords = fragPositionInLightSpace.xyz / fragPositionInLightSpace.w;

    projectionCoords = projectionCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projectionCoords.xy).r;

    float currentDepth = projectionCoords.z;

    float shadow = currentDepth - shadowMapBias > closestDepth ? 1.0 : 0.0;  
    
    return shadow;
}