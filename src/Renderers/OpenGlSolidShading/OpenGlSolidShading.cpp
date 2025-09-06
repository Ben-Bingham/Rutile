#include "OpenGlSolidShading.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Rutile {
    OpenGlSolidShading::OpenGlSolidShading() {
        m_SolidShader = std::make_unique<Shader>("assets\\shaders\\OpenGlSolidShading\\solid.vert", "assets\\shaders\\OpenGlSolidShading\\solid.frag");

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    OpenGlSolidShading::~OpenGlSolidShading() {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    void OpenGlSolidShading::Render(RenderTarget& target, const Camera& camera) {
        target.Bind();

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_SolidShader->Bind();

        for (size_t i = 0; i < m_ObjectCount; ++i) {
            m_SolidShader->SetVec3("color", m_Colours[i]);
   
            glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)target.GetSize().x / (float)target.GetSize().y, camera.nearPlane, camera.farPlane);
            glm::mat4 mvp = projection * camera.View() * m_Transforms[i];

            m_SolidShader->SetMat4("mvp", mvp);

            glBindVertexArray(m_VAOs[i]);
            glDrawElements(GL_TRIANGLES, m_IndexCounts[i], GL_UNSIGNED_INT, nullptr);
        }

        target.Unbind();
    }

    void OpenGlSolidShading::SetScene(Scene& scene) {
        m_ObjectCount = scene.objects.size();

        // Clean up old objects
        glDeleteBuffers(static_cast<GLsizei>(m_EBOs.size()), m_EBOs.data());
        glDeleteBuffers(static_cast<GLsizei>(m_VBOs.size()), m_VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(m_VAOs.size()), m_VAOs.data());

        m_VAOs.clear();
        m_VBOs.clear();
        m_EBOs.clear();

        m_IndexCounts.clear();
        m_Colours.clear();
        m_Transforms.clear();

        // Store new Geometry
        m_VAOs.resize(m_ObjectCount);
        m_VBOs.resize(m_ObjectCount);
        m_EBOs.resize(m_ObjectCount);

        m_IndexCounts.resize(m_ObjectCount);
        m_Colours.resize(m_ObjectCount);
        m_Transforms.resize(m_ObjectCount);

        glGenVertexArrays(static_cast<GLsizei>(m_ObjectCount), m_VAOs.data());
        glGenBuffers(static_cast<GLsizei>(m_ObjectCount), m_VBOs.data());
        glGenBuffers(static_cast<GLsizei>(m_ObjectCount), m_EBOs.data());

        for (size_t i = 0; i < m_ObjectCount; ++i) {
            const Mesh& mesh = scene.objects[i].mesh;

            std::vector<Vertex> vertices = mesh.vertices;
            std::vector<Index> indices = mesh.indices;

            glBindVertexArray(m_VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertices.size()) * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOs[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(indices.size()) * sizeof(Index), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            m_IndexCounts[i] = indices.size();

            m_Colours[i] = scene.objects[i].material->diffuse;

            m_Transforms[i] = scene.objects[i].transform;
        }
    }
}