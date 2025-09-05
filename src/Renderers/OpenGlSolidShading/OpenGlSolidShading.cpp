#include "OpenGlSolidShading.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Rutile {
    OpenGlSolidShading::OpenGlSolidShading() {
        m_SolidShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\solid.vert", "assets\\shaders\\renderers\\OpenGl\\solid.frag");

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

        for (const auto& object : m_Scene.objects) {
            glm::mat4 transform = object.transform;

            std::shared_ptr<Material> mat = object.material;

            m_SolidShader->Bind();

            m_SolidShader->SetVec4("color", glm::vec4{ mat->diffuse.x, mat->diffuse.y, mat->diffuse.z, 1.0f });
   
            glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)target.GetSize().x / (float)target.GetSize().y, camera.nearPlane, camera.farPlane);
            glm::mat4 mvp = projection * camera.View() * transform;

            m_SolidShader->SetMat4("mvp", mvp);

            glBindVertexArray(m_VAOs[0]);
            glDrawElements(GL_TRIANGLES, (int)3, GL_UNSIGNED_INT, nullptr); // TODO 3
        }

        target.Unbind();
    }

    void OpenGlSolidShading::SetScene(Scene& scene) {
        m_Scene = scene;

        // Clean up old Geometry
        glDeleteBuffers(static_cast<GLsizei>(m_EBOs.size()), m_EBOs.data());
        glDeleteBuffers(static_cast<GLsizei>(m_VBOs.size()), m_VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(m_VAOs.size()), m_VAOs.data());

        m_VAOs.clear();
        m_VBOs.clear();
        m_EBOs.clear();

        const size_t geometryCount = m_Scene.objects.size();

        m_VAOs.resize(geometryCount);
        m_VBOs.resize(geometryCount);
        m_EBOs.resize(geometryCount);

        glGenVertexArrays(static_cast<GLsizei>(geometryCount), m_VAOs.data());
        glGenBuffers(static_cast<GLsizei>(geometryCount), m_VBOs.data());
        glGenBuffers(static_cast<GLsizei>(geometryCount), m_EBOs.data());

        for (size_t i = 0; i < geometryCount; ++i) {
            const Mesh& mesh = m_Scene.objects[i].mesh;

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
        }
    }
}