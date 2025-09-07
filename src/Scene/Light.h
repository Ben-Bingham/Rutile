#pragma once

#include <glm/glm.hpp>

namespace Rutile {
	class Light { // TODO rename to pointLIght
	public:
		Light(
			glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f },
			glm::vec3 diffuse = glm::vec3{ 1.0f, 1.0f, 1.0f },
			glm::vec3 ambient = glm::vec3{ 1.0f, 1.0f, 1.0f },
			glm::vec3 specular = glm::vec3{ 1.0f, 1.0f, 1.0f },
			float constant = 0.62f, 
			float linear = 0.175f, 
			float quadratic = 0.035f
		);

		glm::vec3 position;

		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;

		float constant;
		float linear;
		float quadratic;
	};

	class DirectionalLight {
	public:
		DirectionalLight(
			glm::vec3 direction = glm::vec3{ -1.0f, -1.0f, -1.0f }, 
			glm::vec3 diffuse = glm::vec3{ 1.0f, 1.0f, 1.0f },
			glm::vec3 ambient = glm::vec3{ 1.0f, 1.0f, 1.0f },
			glm::vec3 specular = glm::vec3{ 1.0f, 1.0f, 1.0f }
		);

		glm::vec3 direction;

		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;
	};
}