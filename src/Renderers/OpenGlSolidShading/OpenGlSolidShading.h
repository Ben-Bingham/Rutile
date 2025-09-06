#pragma once

#include <memory>

#include "Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Utility/OpenGl/Shader.h"

namespace Rutile {
	class OpenGlSolidShading : public Renderer {
	public:
        OpenGlSolidShading();
        OpenGlSolidShading(const OpenGlSolidShading& other) = delete;
        OpenGlSolidShading(OpenGlSolidShading&& other) noexcept = default;
        OpenGlSolidShading& operator=(const OpenGlSolidShading& other) = delete;
        OpenGlSolidShading& operator=(OpenGlSolidShading&& other) noexcept = default;
        ~OpenGlSolidShading() override;

        void Render(RenderTarget& target, const Camera& camera) override;

        void SetScene(Scene& scene) override;

    private:
        // Shaders
        std::unique_ptr<Shader> m_SolidShader;

        // Objects
        size_t m_ObjectCount;

        // This object format allows for single command VAO, VBO and EBO creation
        std::vector<unsigned int> m_VAOs;
        std::vector<unsigned int> m_VBOs;
        std::vector<unsigned int> m_EBOs;
        std::vector<int> m_IndexCounts;
        std::vector<glm::vec3> m_Colours;
        std::vector<glm::mat4> m_Transforms;
	};
}