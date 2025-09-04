#pragma once

#include <glm/glm.hpp>

namespace Rutile {
	class Light {
	public:
		Light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular);
		Light(const Light& other) = default;
		Light(Light&& other) noexcept = default;
		Light& operator=(const Light& other) = default;
		Light& operator=(Light&& other) noexcept = default;
		virtual ~Light() = default;

		glm::vec3 position;

		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular);
		DirectionalLight(const DirectionalLight& other) = default;
		DirectionalLight(DirectionalLight&& other) noexcept = default;
		DirectionalLight& operator=(const DirectionalLight& other) = default;
		DirectionalLight& operator=(DirectionalLight&& other) noexcept = default;
		virtual ~DirectionalLight() override = default;

		glm::vec3 direction;
	};
}