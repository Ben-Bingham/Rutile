#version 330 core

struct Phong {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

out vec4 outFragColor;

uniform Phong phong;
uniform vec3 cameraPosition;

in vec3 normal;
in vec3 fragPosition;

void main() {
	vec3 lightPos = vec3(10.0, 10.0, 10.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 lightDiffuse = lightColor * 0.5;
	vec3 lightAmbient = lightDiffuse * 0.2;
	vec3 lightSpecular = vec3(1.0, 1.0, 1.0);

	// Ambient
	vec3 ambient = lightAmbient * phong.ambient;

    // Diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * (diff * phong.diffuse);
    
    // Specular
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), phong.shininess);
    vec3 specular = lightSpecular * (spec * phong.specular);  
        
    vec3 result = ambient + diffuse + specular;
    outFragColor = vec4(result, 1.0);
}
