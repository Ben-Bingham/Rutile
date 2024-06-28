#include "OpenGlRenderer.h"
#include "Settings/App.h"
#include "utility/GLDebug.h"
#include "imgui.h"

#include <iostream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rutile {
    GLFWwindow* OpenGlRenderer::Init() {
        UpdateProjectionMatrix();

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

        m_SolidShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\solid.vert", "assets\\shaders\\renderers\\OpenGl\\solid.frag");
        m_PhongShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\phong.vert", "assets\\shaders\\renderers\\OpenGl\\phong.frag");
        m_ShadowMappingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\shadowMapping.vert", "assets\\shaders\\renderers\\OpenGl\\shadowMapping.frag");

        glGenFramebuffers(1, &m_ShadowMapFBO);

        glGenTextures(1, &m_ShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowMapWidth, m_ShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapTexture, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Shadow map frame buffer is not complete" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);

        UpdateShadowMapBias();

        return window;
    }

    void OpenGlRenderer::Cleanup(GLFWwindow* window) {
        glDisable(GL_DEPTH_TEST);

        glDeleteTextures(1, &m_ShadowMapTexture);
        glDeleteFramebuffers(1, &m_ShadowMapFBO);

        m_SolidShader.reset();
        m_PhongShader.reset();
        m_ShadowMappingShader.reset();

        glfwDestroyWindow(window);
    }

    void OpenGlRenderer::Render() {

        glViewport(0, 0, m_ShadowMapWidth, m_ShadowMapHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection = glm::ortho(m_DirectionalLightLeft, m_DirectionalLightRight, m_DirectionalLightBottom, m_DirectionalLightTop, m_DirectionalLightNear, m_DirectionalLightFar);

        glm::mat4 lightView = glm::lookAt(m_DirectionalLightPosition, m_DirectionalLightPosition + m_DirectionalLights[0]->direction, glm::vec3{0.0f, 1.0f, 0.0f});

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        m_ShadowMappingShader->Bind();
        m_ShadowMappingShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        for (size_t i = 0; i < m_PacketCount; ++i) {
            if (!m_ValidPackets[i]) {
                continue;
            }

            m_ShadowMappingShader->SetMat4("model", m_Transforms[i]->matrix);

            glBindVertexArray(m_VAOs[i]);
            glDrawElements(GL_TRIANGLES, (int)m_IndexCounts[i], GL_UNSIGNED_INT, nullptr);
        }

        // Render Normal Scene
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < m_PacketCount; ++i) {
            Shader* shaderProgram = nullptr;

            if (!m_ValidPackets[i]) {
                continue;
            }

            switch (m_MaterialTypes[i]) {
                case MaterialType::SOLID: {
                    Solid* solid = dynamic_cast<Solid*>(m_Materials[i]);

                    shaderProgram = m_SolidShader.get();
                    shaderProgram->Bind();

                    m_SolidShader->SetVec3("color", solid->color);
                    break;
                }
                case MaterialType::PHONG: {
                    Phong* phong = dynamic_cast<Phong*>(m_Materials[i]);

                    shaderProgram = m_PhongShader.get();
                    shaderProgram->Bind();

                    m_PhongShader->SetVec3("phong.ambient", phong->ambient);
                    m_PhongShader->SetVec3("phong.diffuse", phong->diffuse);
                    m_PhongShader->SetVec3("phong.specular", phong->specular);

                    m_PhongShader->SetFloat("phong.shininess", phong->shininess);

                    shaderProgram->SetMat4("model", m_Transforms[i]->matrix);

                    m_PhongShader->SetVec3("cameraPosition", App::camera.position);

                    m_PhongShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);

                    m_PhongShader->SetInt("shadowMap", 0);

                    // Lighting
                    m_PhongShader->SetInt("pointLightCount", static_cast<int>(m_PointLights.size()));
                    for (size_t j = 0; j < m_PointLights.size(); ++j) {
                        if (m_PointLights[j] == nullptr) {
                            continue;
                        }

                        std::string prefix = "pointLights[" + std::to_string(j) + "].";

                        m_PhongShader->SetVec3(prefix + "position", m_PointLights[j]->position);

                        m_PhongShader->SetFloat(prefix + "constant", m_PointLights[j]->constant);
                        m_PhongShader->SetFloat(prefix + "linear", m_PointLights[j]->linear);
                        m_PhongShader->SetFloat(prefix + "quadratic", m_PointLights[j]->quadratic);


                        m_PhongShader->SetVec3(prefix + "ambient", m_PointLights[j]->ambient);
                        m_PhongShader->SetVec3(prefix + "diffuse", m_PointLights[j]->diffuse);
                        m_PhongShader->SetVec3(prefix + "specular", m_PointLights[j]->specular);
                    }

                    m_PhongShader->SetInt("directionalLightCount", static_cast<int>(m_DirectionalLights.size()));

                    for (size_t j = 0; j < m_DirectionalLights.size(); ++j) {
                        if (m_DirectionalLights[j] == nullptr) {
                            continue;
                        }

                        std::string prefix = "directionalLights[" + std::to_string(j) + "].";

                        m_PhongShader->SetVec3(prefix + "direction", m_DirectionalLights[j]->direction);

                        m_PhongShader->SetVec3(prefix + "ambient", m_DirectionalLights[j]->ambient);
                        m_PhongShader->SetVec3(prefix + "diffuse", m_DirectionalLights[j]->diffuse);
                        m_PhongShader->SetVec3(prefix + "specular", m_DirectionalLights[j]->specular);
                    }

                    m_PhongShader->SetInt("spotLightCount", static_cast<int>(m_SpotLights.size()));

                    for (size_t j = 0; j < m_SpotLights.size(); ++j) {
                        if (m_SpotLights[j] == nullptr) {
                            continue;
                        }

                        std::string prefix = "spotLights[" + std::to_string(j) + "].";

                        m_PhongShader->SetVec3(prefix + "position", m_SpotLights[j]->position);
                        m_PhongShader->SetVec3(prefix + "direction", m_SpotLights[j]->direction);

                        m_PhongShader->SetFloat(prefix + "cutOff", m_SpotLights[j]->cutOff);
                        m_PhongShader->SetFloat(prefix + "outerCutOff", m_SpotLights[j]->outerCutOff);

                        m_PhongShader->SetFloat(prefix + "constant", m_SpotLights[j]->constant);
                        m_PhongShader->SetFloat(prefix + "linear", m_SpotLights[j]->linear);
                        m_PhongShader->SetFloat(prefix + "quadratic", m_SpotLights[j]->quadratic);

                        m_PhongShader->SetVec3(prefix + "ambient", m_SpotLights[j]->ambient);
                        m_PhongShader->SetVec3(prefix + "diffuse", m_SpotLights[j]->diffuse);
                        m_PhongShader->SetVec3(prefix + "specular", m_SpotLights[j]->specular);
                    }
                    break;
                }
            }

            glm::mat4 mvp = m_Projection * App::camera.View() * m_Transforms[i]->matrix;

            shaderProgram->SetMat4("mvp", mvp);

            glBindVertexArray(m_VAOs[i]);
            glDrawElements(GL_TRIANGLES, (int)m_IndexCounts[i], GL_UNSIGNED_INT, nullptr);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGlRenderer::SetScene(const Scene& scene) {
        // Lights
        m_PointLights.clear();
        m_DirectionalLights.clear();
        m_SpotLights.clear();

        for (size_t i = 0; i < scene.lights.size(); ++i) {
            const LightType type = scene.lightTypes[i];

            switch (type) {
                case LightType::POINT: {
                    m_PointLights.push_back(dynamic_cast<PointLight*>(scene.lights[i]));
                    m_PointLightIndices.push_back(i);
                    break;
                }
                case LightType::DIRECTION: {
                    m_DirectionalLights.push_back(dynamic_cast<DirectionalLight*>(scene.lights[i]));
                    m_DirectionalLightIndices.push_back(i);
                    break;
                }
                case LightType::SPOTLIGHT: {
                    m_SpotLights.push_back(dynamic_cast<SpotLight*>(scene.lights[i]));
                    m_SpotLightIndices.push_back(i);
                    break;
                }
            }
        }

        // Geometry
        glDeleteBuffers(     static_cast<GLsizei>(m_PacketCount), m_EBOs.data());
        glDeleteBuffers(     static_cast<GLsizei>(m_PacketCount), m_VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(m_PacketCount), m_VAOs.data());

        m_VAOs         .clear();
        m_VBOs         .clear();
        m_EBOs         .clear();

        m_IndexCounts  .clear();
        m_Transforms   .clear();

        m_MaterialTypes.clear();
        m_Materials    .clear();

        m_ValidPackets.clear();

        m_PacketCount = scene.packets.size();

        m_VAOs         .resize(m_PacketCount);
        m_VBOs         .resize(m_PacketCount);
        m_EBOs         .resize(m_PacketCount);

        m_IndexCounts  .resize(m_PacketCount);
        m_Transforms   .resize(m_PacketCount);

        m_MaterialTypes.resize(m_PacketCount);
        m_Materials    .resize(m_PacketCount);

        m_ValidPackets.resize(m_PacketCount);

        glGenVertexArrays(static_cast<GLsizei>(m_PacketCount), m_VAOs.data());
        glGenBuffers(     static_cast<GLsizei>(m_PacketCount), m_VBOs.data());
        glGenBuffers(     static_cast<GLsizei>(m_PacketCount), m_EBOs.data());

        for (size_t i = 0; i < m_PacketCount; ++i) {
            std::vector<Vertex> vertices = scene.packets[i].vertexData;

            if (vertices.empty()) {
                m_ValidPackets[i] = false;
            } else {
                m_ValidPackets[i] = true;
            }

            std::vector<Index> indices = scene.packets[i].indexData;

            m_IndexCounts[i] = indices.size();
            m_Transforms[i] = scene.packets[i].transform;

            m_Transforms[i]->CalculateMatrix();

            m_MaterialTypes[i] = scene.packets[i].materialType;
            m_Materials[i] = scene.packets[i].material;

            glBindVertexArray(m_VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOs[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_STATIC_DRAW);

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

    void OpenGlRenderer::WindowResize() {
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        UpdateProjectionMatrix();
    }

    /*
    //void OpenGlRenderer::SetPacket(size_t index, Packet packet) {
    //    if (packet.vertexData.empty()) {
    //        m_ValidPackets[index] = false;
    //    }
    //    else {
    //        m_ValidPackets[index] = true;
    //    }

    //    glDeleteBuffers     (1, &m_EBOs[index]);
    //    glDeleteBuffers     (1, &m_VBOs[index]);
    //    glDeleteVertexArrays(1, &m_VAOs[index]);

    //    glGenVertexArrays(1, &m_VAOs[index]);
    //    glGenBuffers     (1, &m_VBOs[index]);
    //    glGenBuffers     (1, &m_EBOs[index]);

    //    std::vector<Vertex> vertices = packet.vertexData;
    //    std::vector<Index> indices = packet.indexData;

    //    m_IndexCounts[index] = indices.size();
    //    m_Transforms[index] = packet.transform;

    //    m_Transforms[index]->CalculateMatrix();

    //    m_MaterialTypes[index] = packet.materialType;
    //    m_Materials[index] = packet.material;

    //    glBindVertexArray(m_VAOs[index]);

    //    glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[index]);
    //    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOs[index]);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_STATIC_DRAW);

    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    //    glEnableVertexAttribArray(0);

    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //    glEnableVertexAttribArray(1);

    //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    //    glEnableVertexAttribArray(2);

    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    glBindVertexArray(0);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //}

    //void OpenGlRenderer::SetLight(size_t index, LightType type, Light* light) {
    //    bool found = false;
    //    for (auto i : m_PointLightIndices) {
    //        if (i == index) {
    //            
    //            found = true;
    //        }
    //    }

    //    for (auto i : m_DirectionalLightIndices) {
    //        if (i == index) {

    //            found = true;
    //        }
    //    }

    //    for (auto i : m_SpotLightIndices) {
    //        if (i == index) {

    //            found = true;
    //        }
    //    }

    //    //switch (type) {
    //    //    case LightType::POINT: {
    //    //        m_PointLights[index] = dynamic_cast<PointLight*>(light);
    //    //        m_PointLightIndices[index] = index;
    //    //        break;
    //    //    }
    //    //    case LightType::DIRECTION: {
    //    //        m_DirectionalLights[index] = dynamic_cast<DirectionalLight*>(light);
    //    //        break;
    //    //    }
    //    //    case LightType::SPOTLIGHT: {
    //    //        m_SpotLights[index] = dynamic_cast<SpotLight*>(light);
    //    //        break;
    //    //    }
    //    //}
    //}

    //void OpenGlRenderer::AddPacket(Packet packet) {
    //    if (packet.vertexData.empty()) {
    //        m_ValidPackets.push_back(false);
    //    }
    //    else {
    //        m_ValidPackets.push_back(true);
    //    }

    //    unsigned int VAO;
    //    unsigned int VBO;
    //    unsigned int EBO;

    //    glGenVertexArrays(1, &VAO);
    //    glGenBuffers     (1, &VBO);
    //    glGenBuffers     (1, &EBO);

    //    std::vector<Vertex> vertices = packet.vertexData;
    //    std::vector<Index> indices = packet.indexData;

    //    m_IndexCounts.push_back(indices.size());
    //    m_Transforms.push_back(packet.transform);

    //    m_Transforms.back()->CalculateMatrix();

    //    m_MaterialTypes.push_back(packet.materialType);
    //    m_Materials.push_back(packet.material);

    //    glBindVertexArray(VAO);

    //    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_STATIC_DRAW);

    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    //    glEnableVertexAttribArray(0);

    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //    glEnableVertexAttribArray(1);

    //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    //    glEnableVertexAttribArray(2);

    //    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    glBindVertexArray(0);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //    m_VAOs.push_back(VAO);
    //    m_VBOs.push_back(VBO);
    //    m_EBOs.push_back(EBO);

    //    ++m_PacketCount;
    //}

    //void OpenGlRenderer::AddLight(LightType type, Light* light) {
    //    size_t index = m_PointLights.size() + m_DirectionalLights.size() + m_SpotLights.size();

    //    switch (type) {
    //        case LightType::POINT: {
    //            m_PointLights.push_back(dynamic_cast<PointLight*>(light));
    //            m_PointLightIndices.push_back(index);
    //            break;
    //        }
    //        case LightType::DIRECTION: {
    //            m_DirectionalLights.push_back(dynamic_cast<DirectionalLight*>(light));
    //            m_DirectionalLightIndices.push_back(index);
    //            break;
    //        }
    //        case LightType::SPOTLIGHT: {
    //            m_SpotLights.push_back(dynamic_cast<SpotLight*>(light));
    //            m_SpotLightIndices.push_back(index);
    //            break;
    //        }
    //    }
    //}
    */

    void OpenGlRenderer::UpdatePacketTransform(size_t index) {
        m_Transforms[index]->CalculateMatrix();
    }

    void OpenGlRenderer::UpdateFieldOfView() {
        UpdateProjectionMatrix();
    }

    void OpenGlRenderer::UpdateNearPlane() {
        UpdateProjectionMatrix();
    }

    void OpenGlRenderer::UpdateFarPlane() {
        UpdateProjectionMatrix();
    }

    void OpenGlRenderer::UpdateProjectionMatrix() {
        m_Projection = glm::mat4{ 1.0f };
        m_Projection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
    }

    void OpenGlRenderer::UpdateShadowMapBias() {
        m_PhongShader->Bind();
        m_PhongShader->SetInt("shadowMapBiasMode", App::settings.shadowMapBiasMode);

        m_PhongShader->SetFloat("shadowMapBias", App::settings.shadowMapBias);

        m_PhongShader->SetFloat("dynamicShadowMapBiasMin", App::settings.dynamicShadowMapBiasMin);
        m_PhongShader->SetFloat("dynamicShadowMapBiasMax", App::settings.dynamicShadowMapBiasMax);
    }

    void OpenGlRenderer::ProvideLightVisualization(size_t i) {
        for (const auto index : m_DirectionalLightIndices) {
            if (index == i) {
                if (ImGui::TreeNode("Shadow Map")) {
                    ImGui::DragFloat3("Position", glm::value_ptr(m_DirectionalLightPosition));

                    ImGui::Text("Texture");
                    ImGui::DragInt("Shadow Map Width", &m_ShadowMapWidth);
                    ImGui::DragInt("Shadow Map Height", &m_ShadowMapHeight);

                    ImGui::Text("Frustum");
                    ImGui::DragFloat("Left", &m_DirectionalLightLeft);
                    ImGui::DragFloat("Right", &m_DirectionalLightRight);
                    ImGui::DragFloat("Bottom", &m_DirectionalLightBottom);
                    ImGui::DragFloat("Top", &m_DirectionalLightTop);

                    ImGui::DragFloat("Near Plane", &m_DirectionalLightNear);
                    ImGui::DragFloat("Far Plane", &m_DirectionalLightFar);

                    ImGui::Image((ImTextureID)m_ShadowMapTexture, ImVec2{ (float)m_ShadowMapWidth, (float)m_ShadowMapHeight }, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });

                    ImGui::TreePop();
                }
                break;
            }
        }
    }
}