#pragma once
#include <glm/vec3.hpp>

namespace Rutile {
    enum class MaterialType {
        SOLID,
        PHONG
    };

    class Material {
    public:
        Material() = default;
        Material(const Material& other) = default;
        Material(Material&& other) noexcept = default;
        Material& operator=(const Material& other) = default;
        Material& operator=(Material&& other) noexcept = default;
        virtual ~Material() = default;
    };

    class Solid : public Material {
    public:
        Solid() = default;
        Solid(const Solid& other) = default;
        Solid(Solid&& other) noexcept = default;
        Solid& operator=(const Solid& other) = default;
        Solid& operator=(Solid&& other) noexcept = default;
        ~Solid() override = default;

        glm::vec3 color;
    };

    class Phong : public Material {
    public:
        Phong() = default;
        Phong(const Phong& other) = default;
        Phong(Phong&& other) noexcept = default;
        Phong& operator=(const Phong& other) = default;
        Phong& operator=(Phong&& other) noexcept = default;
        ~Phong() override = default;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    };
}