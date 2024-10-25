#include "VoxelRayTracing.h"
#include "imgui.h"
#include <bitset>
#include <iostream>

#include "GUI/ImGuiUtil.h"

#include "Settings/App.h"

#include "Utility/events/Events.h"
#include "Utility/OpenGl/GLDebug.h"

namespace Rutile {
    uint32_t SetBit(uint32_t value, size_t n) {
        return value |= (1 << n);
    }

    GLFWwindow* VoxelRayTracing::Init() {
        uint32_t i = 293;
        std::cout << "I: " << std::bitset<32>(i) << std::endl;

        //uint32_t l = SetBit(i, 0);
        //std::cout << "L: " << std::bitset<32>(l) << std::endl;

        //uint32_t r = SetBit(i, 31);;
        //std::cout << "R: " << std::bitset<32>(r) << std::endl;



        // The blocks of the chunk
        //i = SetBit(i, 24);
        //i = SetBit(i, 26);
        //i = SetBit(i, 29);
        //i = SetBit(i, 31);

        //std::cout << "I: " << std::bitset<32>(i) << std::endl;

        //std::cout << std::bitset<32>(i >> 24) << std::endl;

        //uint32_t index = i << 8;
        //index = index >> 8;
        //std::cout << "Index: " << std::bitset<32>(index) << std::endl;

        //std::cout << index << std::endl;









        m_RendererLoadTime = std::chrono::steady_clock::now();

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

        m_BlockData = std::make_unique<SSBO<unsigned char>>(0);
        m_VoxelSSBO = std::make_unique<SSBO<Voxel>>(5);

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

        m_VoxelRayTracingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\VoxelRayTracing\\VoxelRayTracing.vert", "assets\\shaders\\renderers\\VoxelRayTracing\\VoxelRayTracing.frag");
        m_RenderingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\VoxelRayTracing\\Rendering.vert", "assets\\shaders\\renderers\\VoxelRayTracing\\Rendering.frag");

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

        m_VoxelRayTracingShader->Bind();

        ResetAccumulatedPixelData();

        CreateOctree();

        return window;
    }

    void VoxelRayTracing::CreateOctree() {
        /*
         *   ---------                     ---------
         *   | 0 | 1 |                     | 4 | 5 |
         *   ---------> +X                 ---------> +X
         *   | 2 | 3 |                     | 6 | 7 |
         *   ---------                     ---------
         *       V                             V
         *       +Z                            +Z
         */

        voxels.clear();
        Voxel root{ glm::vec3{ -1.0f }, glm::vec3{ 1.0f }, 1, 2, 3, 4, 5, 6, 7, 8, true };

        if (!m_Child0) { root.k0 = -1; }
        if (!m_Child1) { root.k1 = -1; }
        if (!m_Child2) { root.k2 = -1; }
        if (!m_Child3) { root.k3 = -1; }
        if (!m_Child4) { root.k4 = -1; }
        if (!m_Child5) { root.k5 = -1; }
        if (!m_Child6) { root.k6 = -1; }
        if (!m_Child7) { root.k7 = -1; }

        voxels.push_back(root);

        for (int i = 0; i < 8; ++i) {
            Voxel vox{};

            vox.hasKids = false;

            glm::vec3 minB;
            glm::vec3 maxB;

            float width = root.maxBound.x - root.minBound.x; // Needs to be the same regardless of what coord is used, but this is just easy
            float halfW = width / 2.0f;

            switch (i) {
            case 0:
                minB = root.minBound;
                maxB = minB + halfW;
                break;
            case 1:
                minB = root.minBound + glm::vec3{ halfW, 0.0f, 0.0f };
                maxB = minB + halfW;
                break;
            case 2:
                minB = root.minBound + glm::vec3{ 0.0f, 0.0f, halfW };
                maxB = minB + halfW;
                break;
            case 3:
                minB = root.minBound + glm::vec3{ halfW, 0.0f, halfW };
                maxB = minB + halfW;
                break;
            case 4:
                minB = root.minBound + glm::vec3{ 0.0f, halfW, 0.0f };
                maxB = minB + halfW;
                break;
            case 5:
                minB = root.minBound + glm::vec3{ halfW, halfW, 0.0f };
                maxB = minB + halfW;
                break;
            case 6:
                minB = root.minBound + glm::vec3{ 0.0f, halfW, halfW };
                maxB = minB + halfW;
                break;
            case 7:
                minB = root.minBound + glm::vec3{ halfW, halfW, halfW };
                maxB = minB + halfW;
                break;
            }

            vox.minBound = minB;
            vox.maxBound = maxB;

            voxels.push_back(vox);
        }

        //Voxel k0{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k1{ glm::vec3{ 0.5f }, glm::vec3{ 1.0f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k2{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k3{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k4{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k5{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k6{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };
        //Voxel k7{ glm::vec3{ 0.0f }, glm::vec3{ 0.5f }, -1, -1, -1, -1, -1, -1, -1, -1, false };


        //voxels.resize(3);
        //voxels[0] = root;
        //voxels[1] = k0;
        //voxels[2] = k1;

        m_VoxelSSBO->SetData(voxels);
    }

    void VoxelRayTracing::Cleanup(GLFWwindow* window) {
        m_VoxelRayTracingShader.reset();
        m_RenderingShader.reset();

        m_BlockData.reset();

        glfwDestroyWindow(window);
    }

    void VoxelRayTracing::Notify(Event* event) {
        if (EVENT_IS(event, WindowResize)) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
            glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);

            ResetAccumulatedPixelData();
        }
        if (EVENT_IS(event, CameraUpdate) ||
            EVENT_IS(event, ObjectTransformUpdate) ||
            EVENT_IS(event, ObjectMaterialUpdate)) {

            ResetAccumulatedPixelData();
        }
    }
    void VoxelRayTracing::Render() {
        m_VoxelRayTracingShader->Bind();

        ++m_FrameCount;

        // Render into accumulation framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        m_VoxelRayTracingShader->Bind();

        const glm::mat4 cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);

        const glm::mat4 inverseView = glm::inverse(App::camera.View());

        m_VoxelRayTracingShader->SetFloat("miliTime", (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_RendererLoadTime).count() / 1000.0f);
        m_VoxelRayTracingShader->SetInt("screenWidth", App::screenWidth);
        m_VoxelRayTracingShader->SetInt("screenHeight", App::screenHeight);

        m_VoxelRayTracingShader->SetMat4("invView", inverseView);
        m_VoxelRayTracingShader->SetMat4("invProjection", inverseProjection);
        m_VoxelRayTracingShader->SetVec3("cameraPosition", App::camera.position);

        m_VoxelRayTracingShader->SetInt("maxBounces", App::settings.maxBounces);

        m_VoxelRayTracingShader->SetBool("hardCoded0", m_Child0);
        m_VoxelRayTracingShader->SetBool("hardCoded1", m_Child1);
        m_VoxelRayTracingShader->SetBool("hardCoded2", m_Child2);
        m_VoxelRayTracingShader->SetBool("hardCoded3", m_Child3);
        m_VoxelRayTracingShader->SetBool("hardCoded4", m_Child4);
        m_VoxelRayTracingShader->SetBool("hardCoded5", m_Child5);
        m_VoxelRayTracingShader->SetBool("hardCoded6", m_Child6);
        m_VoxelRayTracingShader->SetBool("hardCoded7", m_Child7);

        hardCodedOctree = 0;
        hardCodedOctree = m_Child0 ? SetBit(hardCodedOctree, 0) : hardCodedOctree;
        hardCodedOctree = m_Child1 ? SetBit(hardCodedOctree, 1) : hardCodedOctree;
        hardCodedOctree = m_Child2 ? SetBit(hardCodedOctree, 2) : hardCodedOctree;
        hardCodedOctree = m_Child3 ? SetBit(hardCodedOctree, 3) : hardCodedOctree;
        hardCodedOctree = m_Child4 ? SetBit(hardCodedOctree, 4) : hardCodedOctree;
        hardCodedOctree = m_Child5 ? SetBit(hardCodedOctree, 5) : hardCodedOctree;
        hardCodedOctree = m_Child6 ? SetBit(hardCodedOctree, 6) : hardCodedOctree;
        hardCodedOctree = m_Child7 ? SetBit(hardCodedOctree, 7) : hardCodedOctree;

        m_VoxelRayTracingShader->SetInt("hardCodedOctree", hardCodedOctree);

        CreateOctree();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
        m_VoxelRayTracingShader->SetInt("accumulationBuffer", 0);

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

        m_RenderingShader->SetInt("frameCount", (int)m_FrameCount);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void VoxelRayTracing::LoadScene() {
        ResetAccumulatedPixelData();
    }

    void VoxelRayTracing::ProvideLocalRendererSettings() {
        ImGui::Checkbox("0", &m_Child0);
        ImGui::Checkbox("1", &m_Child1);
        ImGui::Checkbox("2", &m_Child2);
        ImGui::Checkbox("3", &m_Child3);
        ImGui::Checkbox("4", &m_Child4);
        ImGui::Checkbox("5", &m_Child5);
        ImGui::Checkbox("6", &m_Child6);
        ImGui::Checkbox("7", &m_Child7);

        ResetAccumulatedPixelData();
    }

    void VoxelRayTracing::ResetAccumulatedPixelData() {
        m_FrameCount = 0;

        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

        std::vector<glm::vec4> clearData{ };
        clearData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);
        for (auto& val : clearData) {
            val = glm::vec4{ 0.0f };
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, clearData.data());
    }
}