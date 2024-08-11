#include "SceneFactory.h"
#include <fstream>

#include "Settings/App.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/Logger.hpp>

#include "Utility/Random.h"

namespace Rutile {
    Scene& SceneFactory::GetScene() {
        return m_Scene;
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

        m_Scene.objects.push_back(obj);
    }

    void SceneFactory::Add(const Geometry& geometry, const Transform& transform, const Material& material, const std::string& name) {
        const GeometryIndex geoIndex = m_Scene.geometryBank.Add(geometry);
        const TransformIndex transformIndex = m_Scene.transformBank.Add(transform);
        const MaterialIndex materialIndex = m_Scene.materialBank.Add(material);

        Add(geoIndex, transformIndex, materialIndex, name);
    }

    void SceneFactory::Add(GeometryIndex geometry, const Transform& transform, const Material& material, const std::string& name) {
        Add(geometry, m_Scene.transformBank.Add(transform), m_Scene.materialBank.Add(material), name);
    }

    void SceneFactory::Add(GeometryFactory::Primitive primitive, const Transform& transform, const Material& material, const std::string& name) {
        Add(GeometryFactory::Construct(primitive), transform, material, name);
    }

    void SceneFactory::Add(const Geometry& geometry, const Transform& transform, MaterialFactory::Color color, const std::string& name) {
        Add(geometry, transform, MaterialFactory::Construct(color), name);
    }

    void SceneFactory::Add(const Geometry& geometry, const Transform& transform, MaterialIndex material, const std::string& name) {
        Add(m_Scene.geometryBank.Add(geometry), m_Scene.transformBank.Add(transform), material, name);
    }

    void SceneFactory::Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialFactory::Color color, const std::string& name) {
        Add(GeometryFactory::Construct(primitive), transform, MaterialFactory::Construct(color), name);
    }

    void SceneFactory::Add(GeometryFactory::Primitive primitive, const Transform& transform, MaterialIndex material, const std::string& name) {
        Add(m_Scene.geometryBank.Add(GeometryFactory::Construct(primitive)), m_Scene.transformBank.Add(transform), material, name);
    }

    void SceneFactory::Add(const PointLight& pointLight) {
        m_Scene.pointLights.push_back(pointLight);
    }

    void SceneFactory::Add(const DirectionalLight& light) {
        m_Scene.directionalLight = light;
        m_Scene.m_EnableDirectionalLight = true;
    }

    void SceneFactory::Add(const std::string& path, TransformIndex transform, MaterialIndex materialIndex) {
        Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);

        std::string fileName = path.substr(path.find_last_of("\\"), path.end() - path.begin());
        const std::string modelName = fileName.substr(1, fileName.end() - fileName.begin());

        std::cout << "Loading model: " << modelName << std::endl;

        std::string modelNameNoFileType = fileName.substr(1, fileName.find_last_of('.') - 1);

        std::string modelPath = path;
        bool exportToBin = true;

        std::ifstream f{ "assets\\models\\bin\\" + modelNameNoFileType + ".assbin" };
        if (f.good()) {
            std::cout << "Found binary version of model: " << modelName << std::endl;

            modelPath = "assets\\models\\bin\\" + modelNameNoFileType + ".assbin";
            exportToBin = false;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR: ASSIMP failed to load model with path:\n" << path << "\n" << importer.GetErrorString() << std::endl;
        }

        if (exportToBin) {
            std::cout << "Could not find binary version of model" << std::endl;
            std::cout << "Creating binary version of: " << modelName << std::endl;
            Assimp::Exporter exporter;

            if (exporter.Export(scene, "assbin", "assets/models/bin/" + modelNameNoFileType + ".assbin") != aiReturn_SUCCESS) {
                std::cout << "ERROR: Failed to create binary version of the model: " << modelName << "\nError string: " << exporter.GetErrorString() << std::endl;
            }
        }

        LoadAssimpNode(scene->mRootNode, scene, transform, materialIndex);

        Assimp::DefaultLogger::kill();
    }

    void SceneFactory::Add(const std::string& path, const Transform& transform) {
        Add(path, m_Scene.transformBank.Add(transform), -1);
    }

    void SceneFactory::Add(const std::string& path, const Transform& transform, const Material& material) {
        Add(path, m_Scene.transformBank.Add(transform), m_Scene.materialBank.Add(material));
    }

    void SceneFactory::Add(const std::string& path, const Transform& transform, MaterialIndex material) {
        Add(path, m_Scene.transformBank.Add(transform), material);
    }

    void SceneFactory::LoadAssimpNode(const aiNode* node, const aiScene* scene, TransformIndex transform, MaterialIndex materialIndex) {
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

            const GeometryIndex geoIndex = m_Scene.geometryBank.Add(Geometry{ "Custom Model Geometry", vertices, indices, Geometry::GeometryType::MODEL }); // TODO change name
            if (materialIndex == std::numeric_limits<size_t>::max()) {
                materialIndex = m_Scene.materialBank.Add(MaterialFactory::Construct(RandomVec3()));
            }

            Add(geoIndex, transform, materialIndex);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            LoadAssimpNode(node->mChildren[i], scene, transform, materialIndex);
        }
    }
}