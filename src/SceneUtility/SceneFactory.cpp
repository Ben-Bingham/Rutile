#include "SceneFactory.h"

#include "Settings/App.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

#include "Utility/Random.h"

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

    void SceneFactory::Add(const std::string& path, TransformIndex transform) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR: ASSIMP failed to load model with path:\n" << path << "\n" << importer.GetErrorString() << std::endl;
        }

        LoadAssimpNode(scene->mRootNode, scene, transform);
    }

    void SceneFactory::Add(const std::string& path, const Transform& transform) {
        Add(path, App::transformBank.Add(transform));
    }

    void SceneFactory::LoadAssimpNode(const aiNode* node, const aiScene* scene, TransformIndex transform) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            std::vector<Vertex> vertices;
            vertices.resize((size_t)mesh->mNumVertices);

            for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
                glm::vec3 pos;
                pos.x = mesh->mVertices[j].x;
                pos.y = mesh->mVertices[j].y;
                pos.z = mesh->mVertices[j].z;

                glm::vec3 normal;
                normal.x = mesh->mNormals[j].x;
                normal.y = mesh->mNormals[j].y;
                normal.z = mesh->mNormals[j].z;

                glm::vec2 texCoords;
                if (mesh->mTextureCoords[0]) {
                    texCoords.x = mesh->mTextureCoords[0][j].x;
                    texCoords.y = mesh->mTextureCoords[0][j].y;
                }
                else {
                    texCoords = glm::vec2{ 0.0f, 0.0f };
                }

                vertices[j] = Vertex{ pos, normal, texCoords };
            }

            std::vector<Index> indices;
            for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
                aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                    indices.push_back(face.mIndices[k]);
                }
            }

            const GeometryIndex geoIndex = App::geometryBank.Add(Geometry{ "filler", vertices, indices });
            const MaterialIndex materialIndex = App::materialBank.Add(MaterialFactory::Construct(RandomVec3()));

            Add(geoIndex, transform, materialIndex);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            LoadAssimpNode(node->mChildren[i], scene, transform);
        }
    }
}