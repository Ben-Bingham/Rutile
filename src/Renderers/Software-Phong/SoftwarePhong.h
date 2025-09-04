#pragma once

//#include "renderers/Renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

namespace Rutile {
    //class SoftwarePhong : public Renderer {
    //public:
    //    GLFWwindow* Init() override;
    //    void Cleanup(GLFWwindow* window) override;
    //    void Render() override;

    //    void Notify(Event* event) override;

    //    void LoadScene() override;

    //private:
    //    // Presenting Image
    //    unsigned int m_ShaderProgram{ 0 };

    //    unsigned int m_VAO{ 0 };
    //    unsigned int m_VBO{ 0 };
    //    unsigned int m_EBO{ 0 };

    //    unsigned int m_ScreenTexture{ 0 };

    //    // TODO
    //    const char* vertexShaderSource = \
    //        "#version 330 core\n"
    //        "\n"
    //        "layout (location = 0) in vec3 inPos;\n"
    //        "layout (location = 1) in vec2 inUv;\n"
    //        "\n"
    //        "out vec2 uv;\n"
    //        "\n"
    //        "void main() {\n"
    //        "   gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);\n"
    //        "   uv = inUv;\n"
    //        "}\n\0";

    //    const char* fragmentShaderSource = \
    //        "#version 330 core\n"
    //        "\n"
    //        "out vec4 outFragColor;\n"
    //        "\n"
    //        "in vec2 uv;\n"
    //        "\n"
    //        "uniform sampler2D tex;\n"
    //        "\n"
    //        "void main() {\n"
    //        "   outFragColor = texture(tex, uv);\n"
    //        "}\n\0";

    //    std::vector<float> vertices = {
    //        // Positions              // Uvs
    //        -1.0f, -1.0f, 0.0f,       0.0f, 0.0f,
    //        -1.0f,  1.0f, 0.0f,       0.0f, 1.0f,
    //         1.0f,  1.0f, 0.0f,       1.0f, 1.0f,
    //         1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
    //    };

    //    std::vector<unsigned int> indices = {
    //        0, 1, 2,
    //        0, 2, 3
    //    };
    //};
}