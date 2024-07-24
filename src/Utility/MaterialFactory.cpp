#include "MaterialFactory.h"

namespace Rutile {
    Material MaterialFactory::Construct(const Material::Solid& solid, const std::string& name) {
        Material::Phong phong;

        phong.diffuse = solid.color;
        phong.ambient = solid.color * 0.5f;
        phong.specular = phong.ambient * 0.25f;

        phong.shininess = 16.0f;

        if (name.empty()) {
            ++m_MaterialNamingIndex;

            return Material{ "Material #" + std::to_string(m_MaterialNamingIndex), solid, phong };
        }

        return Material{ name, solid, phong };
    }

    Material MaterialFactory::Construct(const Material::Phong& phong, const std::string& name) {
        Material::Solid solid;

        solid.color = phong.diffuse; // TODO make this color more accurate

        if (name.empty()) {
            ++m_MaterialNamingIndex;

            return Material{ "Material #" + std::to_string(m_MaterialNamingIndex), solid, phong};
        }

        return Material{ name, solid, phong };
    }

    Material MaterialFactory::Construct(const glm::vec3& color, const std::string& name) {
        Material::Solid solid;
        solid.color = color;

        return Construct(solid, name);
    }

    Material MaterialFactory::Construct(Color color, const std::string& name) {
        glm::vec3 c{ };

        switch (color) {
        case Color::RED:
            c = { 1.0f, 0.0f, 0.0f };
            break;
        case Color::GREEN:
            c = { 0.0f, 1.0f, 0.0f };
            break;
        case Color::BLUE:
            c = { 0.0f, 0.0f, 1.0f };
            break;
        case Color::YELLOW:
            c = { 1.0f, 1.0f, 0.0f };
            break;
        case Color::PINK:
            c = { 1.0f, 0.0f, 1.0f };
            break;
        case Color::CYAN:
            c = { 0.0f, 1.0f, 1.0f };
            break;
        case Color::WHITE:
            c = { 1.0f, 1.0f, 1.0f };
            break;
        case Color::BLACK:
            c = { 0.0f, 0.0f, 0.0f };
            break;
        case Color::GRAY:
            c = { 0.5f, 0.5f, 0.5f };
            break;
        }

        return Construct(c, name);
    }
}