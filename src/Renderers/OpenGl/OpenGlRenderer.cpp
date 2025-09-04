#include "OpenGlRenderer.h"
#include "Settings/App.h"
#include "imgui.h"

#include <iostream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Utility/OpenGl/GLDebug.h"

namespace Rutile {
    void OpenGlRenderer::Init() {
        // Shaders
        m_SolidShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\solid.vert", "assets\\shaders\\renderers\\OpenGl\\solid.frag");
        m_PhongShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\phong.vert", "assets\\shaders\\renderers\\OpenGl\\phong.frag");

        m_OmnidirectionalShadowMappingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\omnidirectionalShadowMapping.vert", "assets\\shaders\\renderers\\OpenGl\\omnidirectionalShadowMapping.frag", "assets\\shaders\\renderers\\OpenGl\\omnidirectionalShadowMapping.geom");
        m_CubeMapVisualizationShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\cubemapVisualization.vert", "assets\\shaders\\renderers\\OpenGl\\cubemapVisualization.frag");

        m_CascadingShadowMapShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\cascadingShadowMapping.vert", "assets\\shaders\\renderers\\OpenGl\\cascadingShadowMapping.frag", "assets\\shaders\\renderers\\OpenGl\\cascadingShadowMapping.geom");
        m_CascadingShadowMapVisualizationShader = std::make_unique<Shader>("assets\\shaders\\renderers\\OpenGl\\cascadingShadowMapVisualization.vert", "assets\\shaders\\renderers\\OpenGl\\cascadingShadowMapVisualization.frag");

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
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_CubeMapVisualizationWidth, m_CubeMapVisualizationHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_CubeMapVisualizationRBO);

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

        // Cascading Shadow maps
        glGenFramebuffers(1, &m_CascadingShadowMapFBO);

        glGenTextures(1, &m_CascadingShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_CascadingShadowMapTexture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, m_CascadingShadowMapWidth, m_CascadingShadowMapHeight, (int)m_CascadeCount, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_CascadingShadowMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_CascadingShadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Error: Framebuffer is not complete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Cascading Shadow map Visualization
        glGenFramebuffers(1, &m_ShadowCascadesVisualizationFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowCascadesVisualizationFBO);

        glGenTextures(1, &m_ShadowCascadesVisualizationTexture);
        glBindTexture(GL_TEXTURE_2D, m_ShadowCascadesVisualizationTexture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ShadowCascadesVisualizationTexture, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_CascadeVisualizationWidth, m_CascadeVisualizationHeight, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenRenderbuffers(1, &m_ShadowCascadesVisualizationRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_ShadowCascadesVisualizationRBO);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_CascadeVisualizationWidth, m_CascadeVisualizationHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_ShadowCascadesVisualizationRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Cascading shadow map visualization framebuffer is not complete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // OpenGl settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        target = std::make_unique<Texture2D>(glm::ivec2{ 100, 100 });
    }

    std::shared_ptr<Texture2D> OpenGlRenderer::Render() {
        if (App::settings.frontFace == WindingOrder::COUNTER_CLOCK_WISE) {
            glFrontFace(GL_CCW);
        }
        else {
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

        //if (App::scene.HasDirectionalLight() && App::settings.directionalShadows && !App::settings.lockCascadeCamera) {
        //    RenderCascadingShadowMaps(); // TODO this should be called sparingly
        //}

        RenderScene();

        if (App::settings.visualizeCascades) {
            //VisualizeShadowCascades();
        }

        if (App::settings.visualizeCascadeLights) {
            //VisualizeCascadeLights();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return target;
    }

    void OpenGlRenderer::Cleanup() {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        // Cascading Shadow maps
        glDeleteTextures(1, &m_CascadingShadowMapTexture);
        glDeleteFramebuffers(1, &m_CascadingShadowMapFBO);

        // Cubemap Visualization
        for (auto& texture : m_CubeMapVisualizationTextures) {
            glDeleteTextures(1, &texture);
        }

        glDeleteFramebuffers(1, &m_CubeMapVisualizationFBO);
        glDeleteRenderbuffers(1, &m_CubeMapVisualizationRBO);

        // Omnidirectional Shadow maps
        for (const auto& cubeMap : m_PointLightCubeMaps) {
            glDeleteTextures(1, &cubeMap);
        }

        glDeleteFramebuffers(1, &m_OmnidirectionalShadowMapFBO);

        // Shaders
        m_SolidShader.reset();
        m_PhongShader.reset();

        m_OmnidirectionalShadowMappingShader.reset();
        m_CubeMapVisualizationShader.reset();

        m_CascadingShadowMapShader.reset();
        m_CascadingShadowMapVisualizationShader.reset();
    }

    void OpenGlRenderer::SetScene(Scene& scene) {
        /*
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_CubeMapVisualizationWidth, m_CubeMapVisualizationHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
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
        glDeleteBuffers(static_cast<GLsizei>(m_EBOs.size()), m_EBOs.data());
        glDeleteBuffers(static_cast<GLsizei>(m_VBOs.size()), m_VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(m_VAOs.size()), m_VAOs.data());

        m_VAOs.clear();
        m_VBOs.clear();
        m_EBOs.clear();

        const size_t geometryCount = App::scene.geometryBank.Size();

        m_VAOs.resize(geometryCount);
        m_VBOs.resize(geometryCount);
        m_EBOs.resize(geometryCount);

        glGenVertexArrays(static_cast<GLsizei>(geometryCount), m_VAOs.data());
        glGenBuffers(static_cast<GLsizei>(geometryCount), m_VBOs.data());
        glGenBuffers(static_cast<GLsizei>(geometryCount), m_EBOs.data());

        for (size_t i = 0; i < geometryCount; ++i) {
            const Geometry& geo = App::scene.geometryBank[i];

            std::vector<Vertex> vertices = geo.vertices;
            std::vector<Index> indices = geo.indices;

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
        */
    }

    void OpenGlRenderer::RenderOmnidirectionalShadowMaps() {
        /*
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
                m_OmnidirectionalShadowMappingShader->SetMat4("model", App::scene.transformBank[object.transform].matrix);

                for (int i = 0; i < 6; ++i) {
                    m_OmnidirectionalShadowMappingShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
                }

                m_OmnidirectionalShadowMappingShader->SetVec3("lightPosition", lightPosition);
                m_OmnidirectionalShadowMappingShader->SetFloat("farPlane", pointLight.shadowMapFarPlane);

                glBindVertexArray(m_VAOs[object.geometry]);
                glDrawElements(GL_TRIANGLES, (int)App::scene.geometryBank[object.geometry].indices.size(), GL_UNSIGNED_INT, nullptr);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ++pointLightIndex;
        }
        */
    }

    void OpenGlRenderer::RenderCascadingShadowMaps() {
        /*
        float distance = abs(App::settings.farPlane - App::settings.nearPlane);

        // There should be one more plane than cascades
        std::vector<float> frustumPlanes;

        frustumPlanes.push_back(App::settings.nearPlane);
        for (int i = 1; i < (int)m_CascadeCount; ++i) {
            frustumPlanes.push_back((distance / (float)m_CascadeCount) * (float)i);
        }
        frustumPlanes.push_back(App::settings.farPlane);

        m_CascadingFrustumPlanes = frustumPlanes;

        m_LightSpaceMatrices.clear();

        m_CascadeCameraProjections.clear();
        m_CascadeLightBoxes.clear();

        for (int i = 0; i < (int)m_CascadeCount; ++i) {
            float nearPlane = frustumPlanes[i];
            float farPlane  = frustumPlanes[i + 1];

            glm::mat4 cameraProjection{ 1.0f };
            cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, nearPlane, farPlane);

            m_CascadeCameraProjections.push_back(cameraProjection * App::camera.View());

            // After we have applied the cameras view and projection matrices to a point in world space it ends up being in the NDC Cube, so its in a box
            // with corners { -1.0f, -1.0f, -1.0f } and { 1.0f, 1.0f, 1.0f }. If we instead go the other direction: We start with a the NDC Cube and than
            // transform back into world space (by applying the inverse of both the cameras projection and view matrices) we can get the corners of
            // the cameras frustum in world space.
            std::vector<glm::vec4> frustumCorners = GetFrustumCornersInWorldSpace(cameraProjection * App::camera.View());

            // frustumCorners is now a list of the corners of the Camera frustum in World space

            glm::vec3 frustumCenter = { 0.0f, 0.0f, 0.0f };
            for (auto corner : frustumCorners) {
                frustumCenter += glm::vec3{ corner };
            }

            frustumCenter /= static_cast<float>(frustumCorners.size());

            // frustumCenter is now the center of the cameras frustum

            glm::mat4 lightView = glm::lookAt(frustumCenter + -glm::normalize(App::scene.directionalLight.direction), frustumCenter, glm::vec3{ 0.0f, 1.0f, 0.0f });

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();

            float minXWorld = std::numeric_limits<float>::max();
            float maxXWorld = std::numeric_limits<float>::lowest();
            float minYWorld = std::numeric_limits<float>::max();
            float maxYWorld = std::numeric_limits<float>::lowest();
            float minZWorld = std::numeric_limits<float>::max();
            float maxZWorld = std::numeric_limits<float>::lowest();

            for (auto corner : frustumCorners) {
                const auto transformedCorner = lightView * glm::vec4{ corner };
                minX = std::min(minX, transformedCorner.x);
                maxX = std::max(maxX, transformedCorner.x);
                minY = std::min(minY, transformedCorner.y);
                maxY = std::max(maxY, transformedCorner.y);
                minZ = std::min(minZ, transformedCorner.z);
                maxZ = std::max(maxZ, transformedCorner.z);

                minXWorld = std::min(minXWorld, corner.x);
                maxXWorld = std::max(maxXWorld, corner.x);
                minYWorld = std::min(minYWorld, corner.y);
                maxYWorld = std::max(maxYWorld, corner.y);
                minZWorld = std::min(minZWorld, corner.z);
                maxZWorld = std::max(maxZWorld, corner.z);
            }

            if (minZ < 0) {
                minZ *= m_ZMinMultiplier;
            }
            else {
                minZ /= m_ZMinMultiplier;
            }

            if (maxZ < 0) {
                maxZ /= m_ZMaxMultiplier;
            }
            else {
                maxZ *= m_ZMaxMultiplier;
            }

            m_CascadeLightBoxes.push_back(std::make_pair<glm::vec3, glm::vec3>({ minXWorld, minYWorld, minZWorld }, { maxXWorld, maxYWorld, maxZWorld }));

            glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

            glm::mat4 lightViewProjection = lightProjection * lightView;

            m_LightSpaceMatrices.push_back(lightViewProjection);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_CascadingShadowMapFBO);

        glViewport(0, 0, m_CascadingShadowMapWidth, m_CascadingShadowMapHeight);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_CascadingShadowMapShader->Bind();

        for (const auto& object : App::scene.objects) {
            m_CascadingShadowMapShader->SetMat4("model", App::scene.transformBank[object.transform].matrix);

            for (int i = 0; i < m_CascadeCount; ++i) {
                m_CascadingShadowMapShader->SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", m_LightSpaceMatrices[i]);
            }

            glBindVertexArray(m_VAOs[object.geometry]);
            glDrawElements(GL_TRIANGLES, (int)App::scene.geometryBank[object.geometry].indices.size(), GL_UNSIGNED_INT, nullptr);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        */
    }

    void OpenGlRenderer::RenderScene() {
        /*
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

            Transform& transform = App::scene.transformBank[object.transform];

            Material mat = App::scene.materialBank[object.material];

            switch (App::settings.materialType) {
                case MaterialType::SOLID: {
                    shaderProgram = m_SolidShader.get();
                    shaderProgram->Bind();

                    // Material
                    shaderProgram->SetVec4("color", glm::vec4{ mat.solid.color, 1.0f });

                    break;
                }
                case MaterialType::PHONG: {
                    shaderProgram = m_PhongShader.get();
                    shaderProgram->Bind();

                    // Material
                    shaderProgram->SetVec3("phong.ambient", mat.phong.ambient);
                    shaderProgram->SetVec3("phong.diffuse", mat.phong.diffuse);
                    shaderProgram->SetVec3("phong.specular", mat.phong.specular);

                    shaderProgram->SetFloat("phong.shininess", mat.phong.shininess);

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

                        shaderProgram->SetBool("directionalShadows", App::settings.directionalShadows);

                        shaderProgram->SetMat4("view", App::camera.View()); // TODO might need to cache viewMatrix

                        shaderProgram->SetInt("cascadeCount", m_CascadeCount);

                        int i = 0;
                        for (const float& plane : m_CascadingFrustumPlanes) {
                            shaderProgram->SetFloat("cascadeFrustumPlanes[" + std::to_string(i) + "]", plane);

                            ++i;
                        }

                        i = 0;
                        for (const glm::mat4& mat : m_LightSpaceMatrices) {
                            shaderProgram->SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", mat);
                            ++i;
                        }

                        shaderProgram->SetFloat("farPlane", App::settings.farPlane);

                        glActiveTexture(GL_TEXTURE4);
                        glBindTexture(GL_TEXTURE_2D_ARRAY, m_CascadingShadowMapTexture);
                        shaderProgram->SetInt("cascadingShadowMap", 4);

                    } else {
                        shaderProgram->SetBool("haveDirectionalLight", false);
                        shaderProgram->SetBool("directionalShadows",   false);
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

        //    glm::mat4 mvp = m_Projection * App::camera.View() * App::scene.transformBank[object.transform].matrix;

        //    shaderProgram->SetMat4("mvp", mvp);

        //    glBindVertexArray(m_VAOs[object.geometry]);
        //    glDrawElements(GL_TRIANGLES, (int)App::scene.geometryBank[object.geometry].indices.size(), GL_UNSIGNED_INT, nullptr);
        //}

        //*/
    }

    std::vector<glm::vec4> OpenGlRenderer::GetFrustumCornersInWorldSpace(const glm::mat4& frustum) {
        glm::mat4 invFrustum = glm::inverse(frustum);

        std::vector<glm::vec4> frustumCorners;
        for (int x = 0; x < 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                for (int z = 0; z < 2; ++z) {
                    glm::vec4 corner = invFrustum * glm::vec4{ (2.0f * x) - 1.0f, (2.0f * y) - 1.0f, (2.0f * z) - 1.0f, 1.0f };

                    frustumCorners.push_back(corner / corner.w);
                }
            }
        }

        return frustumCorners;
    }

    //void OpenGlRenderer::ProjectionMatrixUpdate() {
    //    m_Projection = glm::mat4{ 1.0f };
    //    m_Projection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
    //}

    //void OpenGlRenderer::ProvideLightVisualization(size_t lightIndex) {
    //    if (ImGui::TreeNode("Shadow Map##pointLight")) {
    //        ImGui::Text("Texture");
    //        ImGui::DragInt(("Shadow Map Width##pointLight" + std::to_string(lightIndex)).c_str(), &m_OmnidirectionalShadowMapWidth);
    //        ImGui::DragInt(("Shadow Map Height##pointLight" + std::to_string(lightIndex)).c_str(), &m_OmnidirectionalShadowMapHeight);

    //        ImGui::Text("Depth map");

    //        float vertical = glm::degrees(m_OmnidirectionalShadowMapVisualizationVerticalOffsets[lightIndex]);
    //        float horizontal = glm::degrees(m_OmnidirectionalShadowMapVisualizationHorizontalOffsets[lightIndex]);

    //        ImGui::DragFloat(("Vertical shift##pointLight" + std::to_string(lightIndex)).c_str(), &vertical, 1.0f, -360.0f, 360.0f);
    //        ImGui::DragFloat(("Horizontal shift##pointLight" + std::to_string(lightIndex)).c_str(), &horizontal, 1.0f, -360.0f, 360.0f);

    //        m_OmnidirectionalShadowMapVisualizationVerticalOffsets[lightIndex] = glm::radians(vertical);
    //        m_OmnidirectionalShadowMapVisualizationHorizontalOffsets[lightIndex] = glm::radians(horizontal);

    //        VisualizeCubeMap(lightIndex);
    //        ImGui::Image((ImTextureID)m_CubeMapVisualizationTextures[lightIndex], ImVec2{ (float)m_CubeMapVisualizationWidth, (float)m_CubeMapVisualizationHeight });

    //        ImGui::TreePop();
    //    }
    //}

    //void OpenGlRenderer::ProvideCSMVisualization() {
    //    if (ImGui::TreeNode("Shadow Map##dirLight")) {

    //        ImGui::DragFloat("Frustum Plane Minimum Multiplier", &m_ZMinMultiplier, 0.1f, 0.0f, 100.0f);
    //        ImGui::DragFloat("Frustum Plane Maximum Multiplier", &m_ZMaxMultiplier, 0.1f, 0.0f, 100.0f);

    //        ImGui::DragInt("Cascade Visualization Width##cascadeVisualization", &m_CascadeVisualizationWidth, 1.0f, 0, 4096);
    //        ImGui::DragInt("Cascade Visualization Height##cascadeVisualization", &m_CascadeVisualizationHeight, 1.0f, 0, 4096);

    //        if (ImGui::DragInt("Number of Layers##cascadeVisualization", &m_CascadeCount, 0.01f, 1, m_MaxCascadeCount)) {
    //            glBindTexture(GL_TEXTURE_2D_ARRAY, m_CascadingShadowMapTexture);
    //            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, m_CascadingShadowMapWidth, m_CascadingShadowMapHeight, (int)m_CascadeCount, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    //        }

    //        if (m_CascadeCount != 1) {
    //            ImGui::DragInt("Visualized Layer##cascadeVisualization", &m_DisplayedCascadeLayer, 0.01f, 0, m_CascadeCount - 1);
    //        }

    //        VisualizeCascadeShadowMap(m_DisplayedCascadeLayer);
    //        ImGui::Image((ImTextureID)m_ShadowCascadesVisualizationTexture, ImVec2{ (float)m_CascadeVisualizationWidth, (float)m_CascadeVisualizationHeight }, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });

    //        ImGui::TreePop();
    //    }
    //}

    //void OpenGlRenderer::VisualizeCubeMap(LightIndex lightIndex) {
    //    glBindFramebuffer(GL_FRAMEBUFFER, m_CubeMapVisualizationFBO);

    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_CubeMapVisualizationTextures[lightIndex], 0);

    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //        std::cout << "ERROR: Framebuffer is not complete" << std::endl;
    //    }

    //    std::vector<Vertex> vertices = {
    //        //      Position                         Normal                         Uv
    //        Vertex{ glm::vec3{ -1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
    //        Vertex{ glm::vec3{ -1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } },
    //        Vertex{ glm::vec3{  1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f } },
    //        Vertex{ glm::vec3{  1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
    //    };

    //    std::vector<unsigned int> indices = {
    //        2, 1, 0,
    //        3, 2, 0
    //    };

    //    unsigned int VAO;
    //    unsigned int VBO;
    //    unsigned int EBO;

    //    glGenVertexArrays(1, &VAO);
    //    glGenBuffers(1, &VBO);
    //    glGenBuffers(1, &EBO);

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

    //    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //    m_CubeMapVisualizationShader->Bind();

    //    m_CubeMapVisualizationShader->SetFloat("horizontalModifier", m_OmnidirectionalShadowMapVisualizationVerticalOffsets[lightIndex]);
    //    m_CubeMapVisualizationShader->SetFloat("verticalModifier", m_OmnidirectionalShadowMapVisualizationHorizontalOffsets[lightIndex]);

    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_CUBE_MAP, m_PointLightCubeMaps[lightIndex]);

    //    m_CubeMapVisualizationShader->SetInt("cubeMap", 0);

    //    glBindVertexArray(VAO);
    //    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

    //    glDeleteBuffers(1, &VBO);
    //    glDeleteBuffers(1, &EBO);
    //    glDeleteVertexArrays(1, &VAO);
    //}

    //void OpenGlRenderer::VisualizeCascadeShadowMap(int layer) {
    //    glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowCascadesVisualizationFBO);

    //    glViewport(0, 0, m_CascadeVisualizationWidth, m_CascadeVisualizationHeight);

    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //        std::cout << "ERROR: Framebuffer is not complete" << std::endl;
    //    }

    //    std::vector<Vertex> vertices = {
    //        //      Position                         Normal                         Uv
    //        Vertex{ glm::vec3{ -1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f } },
    //        Vertex{ glm::vec3{ -1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f } },
    //        Vertex{ glm::vec3{  1.0f,  1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f } },
    //        Vertex{ glm::vec3{  1.0f, -1.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f } },
    //    };

    //    std::vector<unsigned int> indices = {
    //        2, 1, 0,
    //        3, 2, 0
    //    };

    //    unsigned int VAO;
    //    unsigned int VBO;
    //    unsigned int EBO;

    //    glGenVertexArrays(1, &VAO);
    //    glGenBuffers(1, &VBO);
    //    glGenBuffers(1, &EBO);

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

    //    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //    m_CascadingShadowMapVisualizationShader->Bind();

    //    m_CascadingShadowMapVisualizationShader->SetInt("layer", layer);

    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D_ARRAY, m_CascadingShadowMapTexture);
    //    m_CascadingShadowMapVisualizationShader->SetInt("shadowMap", 0);

    //    glBindVertexArray(VAO);
    //    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

    //    glDeleteBuffers(1, &VBO);
    //    glDeleteBuffers(1, &EBO);
    //    glDeleteVertexArrays(1, &VAO);
    //}

    //void OpenGlRenderer::VisualizeShadowCascades() {
    //    glDisable(GL_CULL_FACE);
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //    std::vector<glm::vec3> colors {
    //        { 1.0f, 1.0f, 0.0f },
    //        { 1.0f, 0.0f, 1.0f },
    //        { 0.0f, 1.0f, 1.0f },
    //        { 1.0f, 0.0f, 0.0f },
    //        { 0.0f, 1.0f, 0.0f },
    //        { 0.0f, 0.0f, 1.0f },
    //        { 0.5f, 0.5f, 0.0f },
    //        { 0.5f, 0.0f, 0.5f },
    //        { 0.0f, 0.5f, 0.5f },
    //        { 1.0f, 1.0f, 1.0f }
    //    };

    //    int i = 0;
    //    for (const auto& frustum : m_CascadeCameraProjections) {
    //        std::vector<glm::vec4> frustumCorners = GetFrustumCornersInWorldSpace(frustum);

    //        std::vector<float> vertices{ };

    //        for (auto corner : frustumCorners) {
    //            vertices.push_back(corner.x);
    //            vertices.push_back(corner.y);
    //            vertices.push_back(corner.z);
    //        }

    //        std::vector<Index> indices = {
    //            0, 2, 3,
    //            0, 3, 1,
    //            4, 6, 2,
    //            4, 2, 0,
    //            5, 7, 6,
    //            5, 6, 4,
    //            1, 3, 7,
    //            1, 7, 5,
    //            6, 7, 3,
    //            6, 3, 2,
    //            1, 5, 4,
    //            0, 1, 4
    //        };

    //        unsigned int VAO;
    //        unsigned int VBO;
    //        unsigned int EBO;

    //        glGenVertexArrays(1, &VAO);
    //        glGenBuffers(1, &VBO);
    //        glGenBuffers(1, &EBO);

    //        glBindVertexArray(VAO);

    //        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_STATIC_DRAW);

    //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    //        glEnableVertexAttribArray(0);

    //        m_SolidShader->Bind();

    //        m_SolidShader->SetMat4("mvp", m_Projection * App::camera.View());
    //        m_SolidShader->SetVec4("color", glm::vec4{ colors[i], 0.5f });

    //        glBindVertexArray(VAO);
    //        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

    //        glDeleteBuffers(1, &VBO);
    //        glDeleteBuffers(1, &EBO);
    //        glDeleteVertexArrays(1, &VAO);

    //        ++i;
    //    }
    //    glDisable(GL_BLEND);
    //    glEnable(GL_CULL_FACE);
    //}

    //void OpenGlRenderer::VisualizeCascadeLights() {
    //    glDisable(GL_CULL_FACE);
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //    std::vector<glm::vec3> colors {
    //        { 1.0f, 1.0f, 1.0f },
    //        { 0.0f, 0.5f, 0.5f },
    //        { 0.5f, 0.0f, 0.5f },
    //        { 0.5f, 0.5f, 0.0f },
    //        { 0.0f, 0.0f, 1.0f },
    //        { 0.0f, 1.0f, 0.0f },
    //        { 1.0f, 0.0f, 0.0f },
    //        { 0.0f, 1.0f, 1.0f },
    //        { 1.0f, 0.0f, 1.0f },
    //        { 1.0f, 1.0f, 0.0f }
    //    };

    //    int i = 0;
    //    for (const auto& corners : m_CascadeLightBoxes) {
    //        std::vector<float> vertices{ };

    //        glm::vec3 min = corners.first;
    //        glm::vec3 max = corners.second;

    //        vertices.push_back(min.x);
    //        vertices.push_back(min.y);
    //        vertices.push_back(min.z);

    //        vertices.push_back(min.x);
    //        vertices.push_back(min.y);
    //        vertices.push_back(max.z);

    //        vertices.push_back(min.x);
    //        vertices.push_back(max.y);
    //        vertices.push_back(min.z);

    //        vertices.push_back(min.x);
    //        vertices.push_back(max.y);
    //        vertices.push_back(max.z);

    //        vertices.push_back(max.x);
    //        vertices.push_back(min.y);
    //        vertices.push_back(min.z);

    //        vertices.push_back(max.x);
    //        vertices.push_back(min.y);
    //        vertices.push_back(max.z);

    //        vertices.push_back(max.x);
    //        vertices.push_back(max.y);
    //        vertices.push_back(min.z);

    //        vertices.push_back(max.x);
    //        vertices.push_back(max.y);
    //        vertices.push_back(max.z);

    //        std::vector<Index> indices = {
    //            0, 2, 3,
    //            0, 3, 1,
    //            4, 6, 2,
    //            4, 2, 0,
    //            5, 7, 6,
    //            5, 6, 4,
    //            1, 3, 7,
    //            1, 7, 5,
    //            6, 7, 3,
    //            6, 3, 2,
    //            1, 5, 4,
    //            0, 1, 4
    //        };

    //        unsigned int VAO;
    //        unsigned int VBO;
    //        unsigned int EBO;

    //        glGenVertexArrays(1, &VAO);
    //        glGenBuffers(1, &VBO);
    //        glGenBuffers(1, &EBO);

    //        glBindVertexArray(VAO);

    //        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index), indices.data(), GL_STATIC_DRAW);

    //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    //        glEnableVertexAttribArray(0);

    //        m_SolidShader->Bind();

    //        m_SolidShader->SetMat4("mvp", m_Projection * App::camera.View());
    //        m_SolidShader->SetVec4("color", glm::vec4{ colors[i], 0.5f });

    //        glBindVertexArray(VAO);
    //        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

    //        glDeleteBuffers(1, &VBO);
    //        glDeleteBuffers(1, &EBO);
    //        glDeleteVertexArrays(1, &VAO);

    //        ++i;
    //    }
    //    glDisable(GL_BLEND);
    //    glEnable(GL_CULL_FACE);
    //}
}