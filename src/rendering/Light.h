#pragma once
#include <glm/vec3.hpp>

namespace Rutile {
    enum class LightType {
        POINT,
        DIRECTION,
        SPOTLIGHT
    };

    class Light {
    public:
        Light() = default;
        Light(const Light& other) = default;
        Light(Light&& other) noexcept = default;
        Light& operator=(const Light& other) = default;
        Light& operator=(Light&& other) noexcept = default;
        virtual ~Light() = default;
    };

    class PointLight : public Light {
    public:
        PointLight() = default;
        PointLight(const PointLight& other) = default;
        PointLight(PointLight&& other) noexcept = default;
        PointLight& operator=(const PointLight& other) = default;
        PointLight& operator=(PointLight&& other) noexcept = default;
        ~PointLight() override = default;

        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    class DirectionalLight : public Light {
    public:
        DirectionalLight() = default;
        DirectionalLight(const DirectionalLight& other) = default;
        DirectionalLight(DirectionalLight&& other) noexcept = default;
        DirectionalLight& operator=(const DirectionalLight& other) = default;
        DirectionalLight& operator=(DirectionalLight&& other) noexcept = default;
        ~DirectionalLight() override = default;

        glm::vec3 direction;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    class SpotLight : public Light {
    public:
        SpotLight() = default;
        SpotLight(const SpotLight& other) = default;
        SpotLight(SpotLight&& other) noexcept = default;
        SpotLight& operator=(const SpotLight& other) = default;
        SpotLight& operator=(SpotLight&& other) noexcept = default;
        ~SpotLight() override = default;

        glm::vec3 position;
        glm::vec3 direction;

        float cutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
}