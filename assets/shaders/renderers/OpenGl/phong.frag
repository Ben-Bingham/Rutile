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

vec3 pointLightAddition      (PointLight light,       vec3 normal, vec3 viewDir);
vec3 directionalLightAddition(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 spotLightAddition       (SpotLight light,        vec3 normal, vec3 viewDir);

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    for (int i = 0; i < pointLightCount; ++i) {
        result += pointLightAddition(pointLights[i], norm, viewDir);
    }

    for (int i = 0; i < directionalLightCount; ++i) {
        result += directionalLightAddition(directionalLights[i], norm, viewDir);
    }

    for (int i = 0; i < spotLightCount; ++i) {
        result += spotLightAddition(spotLights[i], norm, viewDir);
    }

    outFragColor = vec4(result, 1.0);
}

vec3 pointLightAddition(PointLight light, vec3 normal, vec3 viewDir) {
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

    return (ambient + diffuse + specular);
}

vec3 spotLightAddition(SpotLight light, vec3 normal, vec3 viewDir) {
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

    return (ambient + diffuse + specular);
}