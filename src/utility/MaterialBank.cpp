#include "MaterialBank.h"

#include "Settings/App.h"

namespace Rutile {
    MaterialIndex MaterialBank::Add(const std::string& name, const Solid& solid, const Phong& phong) {
        m_Names.push_back(name);

        m_SolidMaterials.push_back(solid);
        m_PhongMaterials.push_back(phong);

        return m_Names.size() - 1;
    }

    Material* MaterialBank::operator[](MaterialIndex i) {
        switch (App::settings.materialType) {
            case MaterialType::SOLID: {
                return &m_SolidMaterials[i];
            }
            case MaterialType::PHONG: {
                return &m_PhongMaterials[i];
            }
        }

        return nullptr;
    }

    Solid* MaterialBank::GetSolid(MaterialIndex i) {
        return &m_SolidMaterials[i];
    }

    Phong* MaterialBank::GetPhong(MaterialIndex i) {
        return &m_PhongMaterials[i];
    }

    std::string MaterialBank::GetName(MaterialIndex i) {
        return m_Names[i];
    }
}