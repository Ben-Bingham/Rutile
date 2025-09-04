#pragma once

#include <glm/glm.hpp>

#include "Utility/Image.h"

namespace Rutile {
	class Material {
	public:
        Material(glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular);
        Material(const Material& other) = default;
        Material(Material&& other) noexcept = default;
        Material& operator=(const Material& other) = default;
        Material& operator=(Material&& other) noexcept = default;
        virtual ~Material() = default;

        glm::vec3 diffuse;
        glm::vec3 ambient;
        glm::vec3 specular;
	};

    //class TexturedMaterial : public Material {
    //public:
    //    TexturedMaterial(Image diffuse, Image ambient, Image specular);
    //    TexturedMaterial(const TexturedMaterial& other) = default;
    //    TexturedMaterial(TexturedMaterial&& other) noexcept = default;
    //    TexturedMaterial& operator=(const TexturedMaterial& other) = default;
    //    TexturedMaterial& operator=(TexturedMaterial&& other) noexcept = default;
    //    virtual ~TexturedMaterial() override = default;

    //    Image diffuse;
    //    Image ambient;
    //    Image specular;

    //private:
    //    glm::vec3 GetAverageImageColour(Image image);
    //};
}