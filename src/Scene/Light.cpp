#include "Light.h"

namespace Rutile {
	Light::Light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular) 
		: position(position), diffuse(diffuse), ambient(ambient), specular(specular) {

	}

	DirectionalLight::DirectionalLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular) 
		: direction(direction), Light(position, diffuse, ambient, specular) {

	}
}