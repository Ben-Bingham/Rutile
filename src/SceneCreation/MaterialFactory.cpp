#include "MaterialFactory.h"

namespace Rutile {
    Material MaterialFactory::Construct(const glm::vec3& color) {
        return Material{ 
            color,
            color * 0.5f,
            color * 0.2f,
            16.0f
        };
    }

    Material MaterialFactory::Construct(Color color) {
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

        return Construct(c);
    }
}