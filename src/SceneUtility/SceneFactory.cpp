#include "SceneFactory.h"

#include "Settings/App.h"

namespace Rutile {
    Scene SceneFactory::GetScene() {
        return m_CurrentScene;
    }

    void SceneFactory::Add(GeometryIndex geometry, TransformIndex transform, MaterialIndex material, const std::string& name) {
        Object obj;

        obj.name = name;
        obj.geometry = geometry;
        obj.transform = transform;
        obj.material = material;

        if (name.empty()) {
            ++m_ObjectNamingIndex;
            obj.name = "Object #" + std::to_string(m_ObjectNamingIndex);
        }

        m_CurrentScene.objects.push_back(obj);
    }

    void SceneFactory::Add(const Geometry& geometry, const Transform& transform, const Material& material, const std::string& name) {
        const GeometryIndex geoIndex = App::geometryBank.Add(geometry);
        const TransformIndex transformIndex = App::transformBank.Add(transform);
        const MaterialIndex materialIndex = App::materialBank.Add(material);

        Add(geoIndex, transformIndex, materialIndex, name);
    }

    void SceneFactory::Add(GeometryFactory::Primitive primitive, const Transform& transform, const Material& material, const std::string& name) {
        Add(GeometryFactory::Construct(primitive), transform, material, name);
    }

    void SceneFactory::Add(const Geometry& geometry, const Transform& transform, MaterialFactory::Color color, const std::string& name) {
        Add(geometry, transform, MaterialFactory::Construct(color), name);
    }

    void SceneFactory::Add(const Geometry& geometry, const Transform& transform, MaterialIndex material, const std::string& name) {
        Add(App::geometryBank.Add(geometry), App::transformBank.Add(transform), material, name);
    }

    void SceneFactory::Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialFactory::Color color, const std::string& name) {
        Add(GeometryFactory::Construct(primitive), transform, MaterialFactory::Construct(color), name);
    }

    void SceneFactory::Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialIndex material, const std::string& name) {
        Add(App::geometryBank.Add(GeometryFactory::Construct(primitive)), App::transformBank.Add(transform), material, name);
    }

    void SceneFactory::Add(const PointLight& pointLight) {
        m_CurrentScene.pointLights.push_back(pointLight);
    }

    void SceneFactory::Add(const DirectionalLight& light) {
        m_CurrentScene.directionalLight = light;
        m_CurrentScene.m_EnableDirectionalLight = true;
    }
}