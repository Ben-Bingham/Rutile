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

        // Shaders
        m_SolidShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\solid.vert", "assets\\shaders\\renderers\\OpenGl\\solid.frag");
        m_PhongShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\phong.vert", "assets\\shaders\\renderers\\OpenGl\\phong.frag");
        m_DirectionalShadowMappingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\shadowMapping.vert", "assets\\shaders\\renderers\\OpenGl\\shadowMapping.frag");
        m_OmnidirectionalShadowMappingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\omnidirectionalShadowMapping.vert", "assets\\shaders\\renderers\\OpenGl\\omnidirectionalShadowMapping.frag", "assets\\shaders\\renderers\\OpenGl\\omnidirectionalShadowMapping.geom");
        m_CubeMapVisualizationShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\cubemapVisualization.vert", "assets\\shaders\\renderers\\OpenGl\\cubemapVisualization.frag");

        // Omnidirectional Shadow maps
        glGenFramebuffers(1, &m_OmnidirectionalShadowMapFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, m_OmnidirectionalShadowMapFBO);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Cubemap Visualization
        glGenFramebuffers(1, &m_CubeMapVisualizationFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_CubeMapVisualizationFBO);

        glGenRenderbuffers(1, &m_CubeMapVisualizationRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CubeMapVisualizationRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(1024), static_cast<GLsizei>(512));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_CubeMapVisualizationRBO);

        //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        //    std::cout << "ERROR: Framebuffer is not complete" << std::endl;
        //}

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        /*glGenFramebuffers(1, &m_DepthMapFBO);

        glGenTextures(1, &m_ShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_DirectionalShadowMapWidth, m_DirectionalShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapTexture, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Shadow map frame buffer is not complete" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);




        */

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        //UpdateDirectionalShadowMap();
        //UpdateShadowMapMode();
        //UpdateOmnidirectionalShadowMap();

        return window;
    }

    void OpenGlRenderer::Cleanup(GLFWwindow* window) {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        //glDeleteTextures(1, &m_ShadowMapTexture);
        //glDeleteFramebuffers(1, &m_DepthMapFBO);


        // Cubemap Visualization
        glDeleteFramebuffers(1, &m_CubeMapVisualizationFBO);
        //glDeleteTextures(1, &m_CubeMapVisualizationTexture);
        glDeleteRenderbuffers(1, &m_CubeMapVisualizationRBO);

        // Omnidirectional Shadow maps
        glDeleteFramebuffers(1, &m_OmnidirectionalShadowMapFBO);

        // Shaders
        m_SolidShader.reset();
        m_PhongShader.reset();
        m_DirectionalShadowMappingShader.reset();
        m_OmnidirectionalShadowMappingShader.reset();
        m_CubeMapVisualizationShader.reset();

        glfwDestroyWindow(window);
    }

    void OpenGlRenderer::Render() {
        if (App::settings.frontFace == WindingOrder::COUNTER_CLOCK_WISE) {
            glFrontFace(GL_CCW);
        } else {
            glFrontFace(GL_CW);
        }

        /*
        // Directional Shadow Map Rendering
        //if (m_DirectionalLight && App::settings.shadowMapMode == ShadowMapMode::ONE_SHADOW_EMITTER) {
        //    if (App::settings.culledFaceDuringShadowMapping == GeometricFace::FRONT) {
        //        glCullFace(GL_FRONT);
        //    }
        //    else {
        //        glCullFace(GL_BACK);
        //    }

        //    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);

        //    glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);

        //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapTexture, 0);

        //    glViewport(0, 0, m_DirectionalShadowMapWidth, m_DirectionalShadowMapHeight);
        //    glClear(GL_DEPTH_BUFFER_BIT);

        //    glm::mat4 lightProjection = glm::ortho(m_DirectionalLightLeft, m_DirectionalLightRight, m_DirectionalLightBottom, m_DirectionalLightTop, m_DirectionalLightNear, m_DirectionalLightFar);

        //    glm::mat4 lightView = glm::lookAt(m_DirectionalLightPosition, m_DirectionalLightPosition + m_DirectionalLight->direction, glm::vec3{ 0.0f, 1.0f, 0.0f });

        //    m_LightSpaceMatrix = lightProjection * lightView;

        //    m_DirectionalShadowMappingShader->Bind();
        //    m_DirectionalShadowMappingShader->SetMat4("lightSpaceMatrix", m_LightSpaceMatrix);

        //    for (size_t i = 0; i < m_ObjectCount; ++i) {
        //        if (!m_ValidPackets[i]) {
        //            continue;
        //        }

        //        if (m_MaterialTypes[i] != MaterialType::PHONG) {
        //            continue;
        //        }

        //        m_DirectionalShadowMappingShader->SetMat4("model", m_Transforms[i]->matrix);

        //        glBindVertexArray(m_VAOs[i]);
        //        glDrawElements(GL_TRIANGLES, (int)m_IndexCounts[i], GL_UNSIGNED_INT, nullptr);
        //    }
        //}
        */

        RenderOmnidirectionalShadowMaps(); // TODO this should be called sparingly

        RenderScene();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGlRenderer::RenderOmnidirectionalShadowMaps() {
        if (App::settings.culledFaceDuringOmnidirectionalShadowMapping == GeometricFace::FRONT) {
            glCullFace(GL_FRONT);
        } else {
            glCullFace(GL_BACK);
        }

        LightIndex pointLightIndex = 0;
        for (const auto& pointLight : App::scene.pointLights) {
            float aspect = (float)m_OmnidirectionalShadowMapWidth / (float)m_OmnidirectionalShadowMapHeight;
            glm::mat4 shadowMapProjection = glm::perspective(glm::radians(90.0f), aspect, pointLight.shadowMapNearPlane, pointLight.shadowMapFarPlane);

            glm::vec3 lightPosition = pointLight.position;

            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowMapProjection *
                glm::lookAt(lightPosition, lightPosition + glm::vec3{ 1.0,  0.0,  0.0 }, glm::vec3{ 0.0, -1.0,  0.0 }));
            shadowTransforms.push_back(shadowMapProjection *
                glm::lookAt(lightPosition, lightPosition + glm::vec3{ -1.0,  0.0,  0.0 }, glm::vec3{ 0.0, -1.0,  0.0 }));
            shadowTransforms.push_back(shadowMapProjection *
                glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0,  1.0,  0.0 }, glm::vec3{ 0.0,  0.0,  1.0 }));
            shadowTransforms.push_back(shadowMapProjection *
                glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0, -1.0,  0.0 }, glm::vec3{ 0.0,  0.0, -1.0 }));
            shadowTransforms.push_back(shadowMapProjection *
                glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0,  0.0,  1.0 }, glm::vec3{ 0.0, -1.0,  0.0 }));
            shadowTransforms.push_back(shadowMapProjection *
                glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0,  0.0, -1.0 }, glm::vec3{ 0.0, -1.0,  0.0 }));

            glViewport(0, 0, m_OmnidirectionalShadowMapWidth, m_OmnidirectionalShadowMapHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, m_OmnidirectionalShadowMapFBO);

            glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLightCubeMaps[pointLightIndex]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_PointLightCubeMaps[pointLightIndex], 0);

            glClear(GL_DEPTH_BUFFER_BIT);
            m_OmnidirectionalShadowMappingShader->Bind();

            // Render
            for (const auto& object : App::scene.objects) {
                m_OmnidirectionalShadowMappingShader->SetMat4("model", App::transformBank[object.transform].matrix);

                for (int i = 0; i < 6; ++i) {
                    m_OmnidirectionalShadowMappingShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
                }

                m_OmnidirectionalShadowMappingShader->SetVec3("lightPosition", lightPosition);
                m_OmnidirectionalShadowMappingShader->SetFloat("farPlane", pointLight.shadowMapFarPlane);

                glBindVertexArray(m_VAOs[object.geometry]);
                glDrawElements(GL_TRIANGLES, (int)App::geometryBank[object.geometry].indices.size(), GL_UNSIGNED_INT, nullptr);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ++pointLightIndex;
        }
    }

    void OpenGlRenderer::RenderScene() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (App::settings.culledFaceDuringRendering == GeometricFace::FRONT) {
            glCullFace(GL_FRONT);
        }
        else {
            glCullFace(GL_BACK);
        }

        for (const auto& object : App::scene.objects) {
            Shader* shaderProgram = nullptr;

            Transform& transform = App::transformBank[object.transform];

            switch (App::settings.materialType) {
                case MaterialType::SOLID: {
                    shaderProgram = m_SolidShader.get();
                    shaderProgram->Bind();

                    Solid* solid = dynamic_cast<Solid*>(App::materialBank[object.material]);

                    // Material
                    shaderProgram->SetVec3("color", solid->color);

                    break;
                }
                case MaterialType::PHONG: {
                    shaderProgram = m_PhongShader.get();
                    shaderProgram->Bind();

                    Phong* phong = dynamic_cast<Phong*>(App::materialBank[object.material]);

                    // Material
                    shaderProgram->SetVec3("phong.ambient", phong->ambient);
                    shaderProgram->SetVec3("phong.diffuse", phong->diffuse);
                    shaderProgram->SetVec3("phong.specular", phong->specular);

                    shaderProgram->SetFloat("phong.shininess", phong->shininess);

                    // Lighting
                    shaderProgram->SetMat4("model", transform.matrix);

                    shaderProgram->SetVec3("cameraPosition", App::camera.position);

                    // Lights

                    // Directional Light
                    if (App::scene.HasDirectionalLight()) {
                        shaderProgram->SetBool("haveDirectionalLight", true);

                        shaderProgram->SetVec3("directionalLight.direction", App::scene.directionalLight.direction);

                        shaderProgram->SetVec3("directionalLight.ambient", App::scene.directionalLight.ambient);
                        shaderProgram->SetVec3("directionalLight.diffuse", App::scene.directionalLight.diffuse);
                        shaderProgram->SetVec3("directionalLight.specular", App::scene.directionalLight.specular);
                    } else {
                        shaderProgram->SetBool("haveDirectionalLight", false);
                    }

                    // Point Lights
                    shaderProgram->SetInt("pointLightCount", static_cast<int>(App::scene.pointLights.size()));
                    LightIndex lightIndex = 0;
                    for (const auto& pointLight : App::scene.pointLights) {
                        std::string prefix = "pointLights[" + std::to_string(lightIndex) + "].";

                        shaderProgram->SetVec3(prefix + "position", pointLight.position);

                        shaderProgram->SetFloat(prefix + "constant", pointLight.constant);
                        shaderProgram->SetFloat(prefix + "linear", pointLight.linear);
                        shaderProgram->SetFloat(prefix + "quadratic", pointLight.quadratic);

                        shaderProgram->SetVec3(prefix + "ambient", pointLight.ambient);
                        shaderProgram->SetVec3(prefix + "diffuse", pointLight.diffuse);
                        shaderProgram->SetVec3(prefix + "specular", pointLight.specular);

                        shaderProgram->SetFloat(prefix + "farPlane", pointLight.shadowMapFarPlane);

                        glActiveTexture(GL_TEXTURE0 + static_cast<int>(lightIndex));
                        glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLightCubeMaps[lightIndex]);
                        switch (lightIndex) {
                        case 0:
                            shaderProgram->SetInt("pointLightCubeMap0", static_cast<int>(lightIndex));
                            break;
                        case 1:
                            shaderProgram->SetInt("pointLightCubeMap1", static_cast<int>(lightIndex));
                            break;
                        case 2:
                            shaderProgram->SetInt("pointLightCubeMap2", static_cast<int>(lightIndex));
                            break;
                        case 3:
                            shaderProgram->SetInt("pointLightCubeMap3", static_cast<int>(lightIndex));
                            break;
                        }
                        ++lightIndex;
                    }

                    // Omnidirectional Shadow map Settings
                    shaderProgram->SetBool("omnidirectionalShadowMaps", App::settings.omnidirectionalShadowMaps);

                    shaderProgram->SetFloat("omnidirectionalShadowMapBias", App::settings.omnidirectionalShadowMapBias);

                    shaderProgram->SetInt("omnidirectionalShadowMapPCFMode", (int)App::settings.omnidirectionalShadowMapPCFMode);

                    shaderProgram->SetInt("omnidirectionalShadowMapSampleCount", App::settings.omnidirectionalShadowMapSampleCount);

                    shaderProgram->SetInt("omnidirectionalShadowMapDiskRadiusMode", (int)App::settings.omnidirectionalShadowMapDiskRadiusMode);
                    shaderProgram->SetFloat("omnidirectionalShadowMapDiskRadius", App::settings.omnidirectionalShadowMapDiskRadius);

                    break;
                }
            }

            /*
             *
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

                    m_PhongShader->SetMat4("lightSpaceMatrix", m_LightSpaceMatrix);

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


                        prefix = "omnidirectionalShadowMaps[" + std::to_string(j) + "]";

                        glActiveTexture(GL_TEXTURE0 + (unsigned int)j);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLightCubeMaps[j]);
                        m_PhongShader->SetInt(prefix, (int)j);

                        m_PhongShader->SetFloat("farPlane", 25.0f); // TODO
                    }

                    glActiveTexture(GL_TEXTURE0 + 5);
                    glBindTexture(GL_TEXTURE_2D, m_ShadowMapTexture);

                    m_PhongShader->SetInt("shadowMap", 0 + 5);

                    if (m_DirectionalLight) {
                        m_PhongShader->SetVec3("directionalLight.direction", m_DirectionalLight->direction);

                        m_PhongShader->SetVec3("directionalLight.ambient", m_DirectionalLight->ambient);
                        m_PhongShader->SetVec3("directionalLight.diffuse", m_DirectionalLight->diffuse);
                        m_PhongShader->SetVec3("directionalLight.specular", m_DirectionalLight->specular);
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

            */

            glm::mat4 mvp = m_Projection * App::camera.View() * App::transformBank[object.transform].matrix;

            shaderProgram->SetMat4("mvp", mvp);

            glBindVertexArray(m_VAOs[object.geometry]);
            glDrawElements(GL_TRIANGLES, (int)App::geometryBank[object.geometry].indices.size(), GL_UNSIGNED_INT, nullptr);
        }
    }

    void OpenGlRenderer::ProjectionMatrixUpdate() {
        m_Projection = glm::mat4{ 1.0f };
        m_Projection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
    }

    void OpenGlRenderer::SignalNewScene() {
        // Lights

        // Cleanup old Point Lights
        for (const auto& cubeMap : m_PointLightCubeMaps) {
            glDeleteTextures(1, &cubeMap);
        }

        m_PointLightCubeMaps.clear();

        m_OmnidirectionalShadowMapVisualizationHorizontalOffsets.clear();
        m_OmnidirectionalShadowMapVisualizationVerticalOffsets.clear();

        for (auto& texture : m_CubeMapVisualizationTextures) {
            glDeleteTextures(1, &texture);
        }

        m_CubeMapVisualizationTextures.clear();

        // Create new Point Lights
        LightIndex pointLightIndex = 0;
        for (const auto& pointLight : App::scene.pointLights) {
            // Cube map
            unsigned int cubeMap;

            glGenTextures(1, &cubeMap);

            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
            for (int i = 0; i < 6; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_OmnidirectionalShadowMapWidth,
                    m_OmnidirectionalShadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            m_PointLightCubeMaps.push_back(cubeMap);

            // Cube map Visualization
            unsigned int cubeMapVisualizationTexture;

            glGenTextures(1, &cubeMapVisualizationTexture);
            glBindTexture(GL_TEXTURE_2D, cubeMapVisualizationTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(1024), static_cast<GLsizei>(512), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            m_CubeMapVisualizationTextures.push_back(cubeMapVisualizationTexture);

            glBindTexture(GL_TEXTURE_2D, 0);

            ++pointLightIndex;
        }

        m_OmnidirectionalShadowMapVisualizationHorizontalOffsets.resize(App::scene.pointLights.size());
        m_OmnidirectionalShadowMapVisualizationVerticalOffsets.resize(App::scene.pointLights.size());

        // Geometry

        // Clean up old Geometry
        glDeleteBuffers(     static_cast<GLsizei>(m_EBOs.size()), m_EBOs.data());
        glDeleteBuffers(     static_cast<GLsizei>(m_VBOs.size()), m_VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(m_VAOs.size()), m_VAOs.data());

        m_VAOs.clear();
        m_VBOs.clear();
        m_EBOs.clear();

        const size_t geometryCount = App::geometryBank.Size();

        m_VAOs.resize(geometryCount);
        m_VBOs.resize(geometryCount);
        m_EBOs.resize(geometryCount);

        glGenVertexArrays(static_cast<GLsizei>(geometryCount), m_VAOs.data());
        glGenBuffers(     static_cast<GLsizei>(geometryCount), m_VBOs.data());
        glGenBuffers(     static_cast<GLsizei>(geometryCount), m_EBOs.data());

        for (size_t i = 0; i < geometryCount; ++i) {
            const Geometry& geo = App::geometryBank[i];

            std::vector<Vertex> vertices = geo.vertices;
            std::vector<Index> indices   = geo.indices;

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

    void OpenGlRenderer::ProvideDirectionalLightVisualization() {
        //if (ImGui::TreeNode("Shadow Map")) {
        //    ImGui::DragFloat3("Position", glm::value_ptr(m_DirectionalLightPosition));
        //
        //    ImGui::Text("Texture");
        //    ImGui::DragInt("Shadow Map Width", &m_DirectionalShadowMapWidth);
        //    ImGui::DragInt("Shadow Map Height", &m_DirectionalShadowMapHeight);
        //
        //    ImGui::Text("Frustum");
        //    ImGui::DragFloat("Left", &m_DirectionalLightLeft);
        //    ImGui::DragFloat("Right", &m_DirectionalLightRight);
        //    ImGui::DragFloat("Bottom", &m_DirectionalLightBottom);
        //    ImGui::DragFloat("Top", &m_DirectionalLightTop);
        //
        //    ImGui::DragFloat("Near Plane", &m_DirectionalLightNear);
        //    ImGui::DragFloat("Far Plane", &m_DirectionalLightFar);
        //
        //    ImGui::Image((ImTextureID)m_ShadowMapTexture, ImVec2{ (float)m_DirectionalShadowMapWidth, (float)m_DirectionalShadowMapHeight }, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });

        //    ImGui::TreePop();
        //}
    }

    void OpenGlRenderer::ProvideLightVisualization(LightIndex lightIndex) {
        if (ImGui::TreeNode("Shadow Map##pointLight")) {
            ImGui::Text("Texture");
            ImGui::DragInt(("Shadow Map Width##pointLight"  + std::to_string(lightIndex)).c_str(), &m_OmnidirectionalShadowMapWidth);
            ImGui::DragInt(("Shadow Map Height##pointLight" + std::to_string(lightIndex)).c_str(), &m_OmnidirectionalShadowMapHeight);

            ImGui::Text("Depth map");

            float vertical =   glm::degrees(m_OmnidirectionalShadowMapVisualizationVerticalOffsets  [lightIndex]);
            float horizontal = glm::degrees(m_OmnidirectionalShadowMapVisualizationHorizontalOffsets[lightIndex]);
            
            ImGui::DragFloat(("Vertical shift##pointLight"   + std::to_string(lightIndex)).c_str(),   &vertical,   1.0f, -360.0f, 360.0f);
            ImGui::DragFloat(("Horizontal shift##pointLight" + std::to_string(lightIndex)).c_str(), &horizontal, 1.0f, -360.0f, 360.0f);

            m_OmnidirectionalShadowMapVisualizationVerticalOffsets  [lightIndex] = glm::radians(vertical);
            m_OmnidirectionalShadowMapVisualizationHorizontalOffsets[lightIndex] = glm::radians(horizontal);

            VisualizeCubeMap(lightIndex);
            ImGui::Image((ImTextureID)m_CubeMapVisualizationTextures[lightIndex], ImVec2{(float)1024, (float)512});

            ImGui::TreePop();
        }
    }

    void OpenGlRenderer::VisualizeCubeMap(LightIndex lightIndex) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_CubeMapVisualizationFBO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_CubeMapVisualizationTextures[lightIndex], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Framebuffer is not complete" << std::endl;
        }

        std::vector<Vertex> vertices = {
            //      Position                         Normal                         Uv
            Vertex{ glm::vec3{ -1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
            Vertex{ glm::vec3{ -1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } },
            Vertex{ glm::vec3{  1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f } },
            Vertex{ glm::vec3{  1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
        };

        std::vector<unsigned int> indices = {
            2, 1, 0,
            3, 2, 0
        };

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        m_CubeMapVisualizationShader->Bind();

        m_CubeMapVisualizationShader->SetFloat("horizontalModifier", m_OmnidirectionalShadowMapVisualizationVerticalOffsets[lightIndex]);
        m_CubeMapVisualizationShader->SetFloat("verticalModifier", m_OmnidirectionalShadowMapVisualizationHorizontalOffsets[lightIndex]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLightCubeMaps[lightIndex]);

        m_CubeMapVisualizationShader->SetInt("cubeMap", 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
}