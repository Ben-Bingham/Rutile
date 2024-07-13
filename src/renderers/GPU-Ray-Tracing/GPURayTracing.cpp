#include "GPURayTracing.h"
#include <iostream>

#include "renderers/OpenGl/utility/GLDebug.h"

#include "Settings/App.h"

namespace Rutile {
    GLFWwindow* GPURayTracing::Init() {
        const char* vertexShaderSource = \
            "#version 330 core\n"
            "\n"
            "layout (location = 0) in vec3 inPos;\n"
            "layout (location = 1) in vec2 inUv;\n"
            "\n"
            "out vec2 uv;\n"
            "\n"
            "void main() {\n"
            "   gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);\n"
            "   uv = inUv;\n"
            "}\n\0";

        const char* fragmentShaderSource = \
            "#version 330 core\n"
            "\n"
            "out vec4 outFragColor;\n"
            "\n"
            "in vec2 uv;\n"
            "\n"
            "uniform sampler2D tex;\n"
            "\n"
            "void main() {\n"
            "   outFragColor = texture(tex, uv);\n"
            "}\n\0";

        std::vector<float> vertices = {
            // Positions
            -1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        GLFWwindow* window = glfwCreateWindow(App::screenWidth, App::screenHeight, App::name.c_str(), nullptr, nullptr);
        glfwShowWindow(window);

        if (!window) {
            std::cout << "ERROR: Failed to create window." << std::endl;
        }

        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK) {
            std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
        }

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

        m_RayTracingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\GPURayTracing\\GPURayTracing.vert", "assets\\shaders\\renderers\\GPURayTracing\\GPURayTracing.frag");

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return window;
    }

    void GPURayTracing::Cleanup(GLFWwindow* window) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);

        m_RayTracingShader.reset();

        glfwDestroyWindow(window);
    }

    void GPURayTracing::Render() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_RayTracingShader->Bind();

        const glm::mat4 cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);

        const glm::mat4 inverseView = glm::inverse(App::camera.View());

        m_RayTracingShader->SetMat4("invView", inverseView);
        m_RayTracingShader->SetMat4("invProjection", inverseProjection);
        m_RayTracingShader->SetVec3("cameraPosition", App::camera.position);

        m_RayTracingShader->SetVec3("backgroundColor", App::settings.backgroundColor);

        int i = 0;
        for (const auto& object : App::scene.objects) {
            const glm::mat4 invModel = glm::inverse(App::transformBank[object.transform].matrix);

            m_RayTracingShader->SetMat4("objects[" + std::to_string(i) + "].invModel", invModel);

            ++i;
        }

        m_RayTracingShader->SetInt("objectCount", App::scene.objects.size());

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void GPURayTracing::WindowResizeEvent() {
        glViewport(0, 0, App::screenWidth, App::screenHeight);
    }
}