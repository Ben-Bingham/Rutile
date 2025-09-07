#include "Light.h"

namespace Rutile {
	Light::Light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular, float constant, float linear, float quadratic)
		: position(position), diffuse(diffuse), ambient(ambient), specular(specular), constant(constant), linear(linear), quadratic(quadratic) {

	}

	DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular)
		: direction(direction), diffuse(diffuse), ambient(ambient), specular(specular) {

	}
}