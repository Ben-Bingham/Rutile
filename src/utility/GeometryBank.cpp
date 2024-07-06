#include "GeometryBank.h"

namespace Rutile {
    GeometryIndex GeometryBank::Add(std::string& name, const Geometry& geometry) {
        m_Names.push_back(name);

        m_Geometries.push_back(geometry);

        return m_Geometries.size() - 1;
    }

    Geometry& GeometryBank::operator[](GeometryIndex i) {
        return m_Geometries[i];
    }

    std::string GeometryBank::GetName(GeometryIndex i) {
        return m_Names[i];
    }

    size_t GeometryBank::Size() {
        return m_Geometries.size();
    }
}