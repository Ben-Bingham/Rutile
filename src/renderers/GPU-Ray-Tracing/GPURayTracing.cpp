#include "GPURayTracing.h"
#include "imgui.h"
#include <iostream>

#include "renderers/OpenGl/utility/GLDebug.h"

#include "Settings/App.h"

namespace Rutile {
    GLFWwindow* GPURayTracing::Init() {
        m_RendererLoadTime = std::chrono::steady_clock::now();

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
        m_RenderingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\GPURayTracing\\Rendering.vert", "assets\\shaders\\renderers\\GPURayTracing\\Rendering.frag");

        // Screen Rectangle
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

        // Accumulation Framebuffer
        glGenFramebuffers(1, &m_AccumulationFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);

        glGenTextures(1, &m_AccumulationTexture);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenRenderbuffers(1, &m_AccumulationRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Accumulation Framebuffer is not complete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ResetAccumulatedPixelData();

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
        ++m_FrameCount;

        // Render into accumulation framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        m_RayTracingShader->Bind();

        const glm::mat4 cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);

        const glm::mat4 inverseView = glm::inverse(App::camera.View());

        m_RayTracingShader->SetFloat("miliTime", (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_RendererLoadTime).count() / 1000.0f);
        m_RayTracingShader->SetInt("screenWidth", App::screenWidth);
        m_RayTracingShader->SetInt("screenHeight", App::screenHeight);

        m_RayTracingShader->SetMat4("invView", inverseView);
        m_RayTracingShader->SetMat4("invProjection", inverseProjection);
        m_RayTracingShader->SetVec3("cameraPosition", App::camera.position);

        m_RayTracingShader->SetVec3("backgroundColor", App::settings.backgroundColor);

        for (size_t i = 0; i < App::materialBank.Size(); ++i) {
            m_RayTracingShader->SetVec3("materialBank[" + std::to_string(i) + "].color", App::materialBank.GetSolid(i)->color);
        }

        int i = 0;
        for (const auto& object : App::scene.objects) {
            const glm::mat4 invModel = glm::inverse(App::transformBank[object.transform].matrix);

            m_RayTracingShader->SetMat4("objects[" + std::to_string(i) + "].model",         App::transformBank[object.transform].matrix);
            m_RayTracingShader->SetMat4("objects[" + std::to_string(i) + "].invModel",      invModel);
            m_RayTracingShader->SetInt ("objects[" + std::to_string(i) + "].materialIndex", (int)object.material);

            ++i;
        }

        m_RayTracingShader->SetInt("objectCount", (int)App::scene.objects.size());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
        m_RayTracingShader->SetInt("accumulationBuffer", 0);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Read from accumulation framebuffer, divide by frame count, and render to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        glClearColor(App::settings.backgroundColor.b, App::settings.backgroundColor.g, App::settings.backgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_RenderingShader->Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
        m_RenderingShader->SetInt("accumulationBuffer", 0);

        m_RenderingShader->SetInt("frameCount", m_FrameCount);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void GPURayTracing::SignalNewScene() {
        ResetAccumulatedPixelData();
    }

    void GPURayTracing::WindowResizeEvent() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);

        ResetAccumulatedPixelData();
    }

    void GPURayTracing::CameraUpdateEvent() {
        ResetAccumulatedPixelData();
    }

    void GPURayTracing::SignalObjectMaterialUpdate(ObjectIndex i) {
        ResetAccumulatedPixelData();
    }

    void GPURayTracing::SignalObjectTransformUpdate(ObjectIndex i) {
        ResetAccumulatedPixelData();
    }

    void GPURayTracing::ProvideLocalRendererSettings() { }

    void GPURayTracing::ResetAccumulatedPixelData() {
        m_FrameCount = 0;

        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

        std::vector<glm::vec4> clearData{ };
        clearData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, clearData.data());
    }
}