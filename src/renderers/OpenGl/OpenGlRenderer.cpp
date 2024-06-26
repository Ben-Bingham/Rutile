#include "OpenGlRenderer.h"
#include "App.h"
#include "GLDebug.h"
#include "imgui.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.inl>

namespace Rutile {
    std::string readShader(const std::string& path) {
        std::string shader;
        std::ifstream file;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);

            std::stringstream shaderStream;
            shaderStream << file.rdbuf();

            file.close();
            shader = shaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR: Could not successfully read shader with path: " << path << "And error: " << e.what() << std::endl;
        }

        return shader;
    }

    unsigned int createShader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexShaderSource = readShader(vertexPath);
        const char* vertexSource = vertexShaderSource.c_str();

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "ERROR: Vertex shader failed to compile:" << std::endl;
            std::cout << infoLog << std::endl;
        }

        std::string fragmentShaderSource = readShader(fragmentPath);
        const char* fragmentSource = fragmentShaderSource.c_str();

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "ERROR: Fragment shader failed to compile:" << std::endl;
            std::cout << infoLog << std::endl;
        }

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cout << "ERROR: Shader program failed to link:" << std::endl;
            std::cout << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    GLFWwindow* OpenGlRenderer::Init() {
        m_Projection = glm::perspective(glm::radians(App::fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::nearPlane, App::farPlane);

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

        m_SolidShader = createShader("assets\\shaders\\renderers\\OpenGl\\solid.vert", "assets\\shaders\\renderers\\OpenGl\\solid.frag");
        m_PhongShader = createShader("assets\\shaders\\renderers\\OpenGl\\phong.vert", "assets\\shaders\\renderers\\OpenGl\\phong.frag");

        glEnable(GL_DEPTH_TEST);

        return window;
    }

    void OpenGlRenderer::Cleanup(GLFWwindow* window) {
        glDisable(GL_DEPTH_TEST);

        glDeleteProgram(m_PhongShader);
        glDeleteProgram(m_SolidShader);

        glfwDestroyWindow(window);
    }

    void OpenGlRenderer::Render() {

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < m_PacketCount; ++i) {
            unsigned int* shaderProgram = nullptr;

            switch (m_MaterialTypes[i]) {
                case MaterialType::SOLID: {
                    Solid* solid = dynamic_cast<Solid*>(m_Materials[i]);

                    shaderProgram = &m_SolidShader;
                    glUseProgram(m_SolidShader);

                    glUniform3fv(glGetUniformLocation(m_SolidShader, "color"), 1, glm::value_ptr(solid->color));
                    break;
                }
                case MaterialType::PHONG: {
                    Phong* phong = dynamic_cast<Phong*>(m_Materials[i]);

                    shaderProgram = &m_PhongShader;
                    glUseProgram(m_PhongShader);

                    glUniform3fv(glGetUniformLocation(m_PhongShader, "phong.ambient"), 1, glm::value_ptr(phong->ambient));
                    glUniform3fv(glGetUniformLocation(m_PhongShader, "phong.diffuse"), 1, glm::value_ptr(phong->diffuse));
                    glUniform3fv(glGetUniformLocation(m_PhongShader, "phong.specular"), 1, glm::value_ptr(phong->ambient));
                    glUniform1f(glGetUniformLocation(m_PhongShader, "phong.shininess"), phong->shininess);

                    glUniformMatrix4fv(glGetUniformLocation(*shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(*m_Transforms[i]));

                    glUniform3fv(glGetUniformLocation(m_PhongShader, "cameraPosition"), 1, glm::value_ptr(App::camera.position));

                    // Lighting
                    glUniform1i(glGetUniformLocation(m_PhongShader, "pointLightCount"), static_cast<int>(m_PointLights.size()));
                    for (size_t j = 0; j < m_PointLights.size(); ++j) {
                        std::string prefix = "pointLights[" + std::to_string(j) + "].";

                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "position").c_str()), 1, glm::value_ptr(m_PointLights[j]->position));

                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "constant").c_str()), m_PointLights[j]->constant);
                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "linear").c_str()), m_PointLights[j]->linear);
                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "quadratic").c_str()), m_PointLights[j]->quadratic);

                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "ambient").c_str()), 1, glm::value_ptr(m_PointLights[j]->ambient));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "diffuse").c_str()), 1, glm::value_ptr(m_PointLights[j]->diffuse));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "specular").c_str()), 1, glm::value_ptr(m_PointLights[j]->specular));
                    }

                    glUniform1i(glGetUniformLocation(m_PhongShader, "directionalLightCount"), static_cast<int>(m_DirectionalLights.size()));
                    for (size_t j = 0; j < m_DirectionalLights.size(); ++j) {
                        std::string prefix = "directionalLights[" + std::to_string(j) + "].";

                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "direction").c_str()), 1, glm::value_ptr(m_DirectionalLights[j]->direction));

                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "ambient").c_str()), 1, glm::value_ptr(m_DirectionalLights[j]->ambient));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "diffuse").c_str()), 1, glm::value_ptr(m_DirectionalLights[j]->diffuse));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "specular").c_str()), 1, glm::value_ptr(m_DirectionalLights[j]->specular));
                    }

                    glUniform1i(glGetUniformLocation(m_PhongShader, "spotLightCount"), static_cast<int>(m_SpotLights.size()));
                    for (size_t j = 0; j < m_SpotLights.size(); ++j) {
                        std::string prefix = "spotLights[" + std::to_string(j) + "].";

                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "position").c_str()), 1, glm::value_ptr(m_SpotLights[j]->position));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "direction").c_str()), 1, glm::value_ptr(m_SpotLights[j]->direction));

                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "cutOff").c_str()), m_SpotLights[j]->cutOff);
                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "outerCutOff").c_str()), m_SpotLights[j]->outerCutOff);

                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "constant").c_str()), m_SpotLights[j]->constant);
                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "linear").c_str()), m_SpotLights[j]->linear);
                        glUniform1f(glGetUniformLocation(m_PhongShader, (prefix + "quadratic").c_str()), m_SpotLights[j]->quadratic);

                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "ambient").c_str()), 1, glm::value_ptr(m_SpotLights[j]->ambient));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "diffuse").c_str()), 1, glm::value_ptr(m_SpotLights[j]->diffuse));
                        glUniform3fv(glGetUniformLocation(m_PhongShader, (prefix + "specular").c_str()), 1, glm::value_ptr(m_SpotLights[j]->specular));
                    }
                    break;
                }
            }

            glm::mat4 mvp = m_Projection * App::camera.View() * *m_Transforms[i];

            glUniformMatrix4fv(glGetUniformLocation(*shaderProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

            glBindVertexArray(m_VAOs[i]);
            glDrawElements(GL_TRIANGLES, (int)m_IndexCounts[i], GL_UNSIGNED_INT, nullptr);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGlRenderer::SetBundle(const Scene& bundle) {
        // Lights
        m_PointLights.clear();
        m_DirectionalLights.clear();
        m_SpotLights.clear();

        for (size_t i = 0; i < bundle.lights.size(); ++i) {
            LightType type = bundle.lightTypes[i];

            switch (type) {
                case LightType::POINT: {
                    m_PointLights.push_back(dynamic_cast<PointLight*>(bundle.lights[i]));
                    break;
                }
                case LightType::DIRECTION: {
                    m_DirectionalLights.push_back(dynamic_cast<DirectionalLight*>(bundle.lights[i]));
                    break;
                }
                case LightType::SPOTLIGHT: {
                    m_SpotLights.push_back(dynamic_cast<SpotLight*>(bundle.lights[i]));
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

        m_PacketCount = bundle.packets.size();

        m_VAOs         .resize(m_PacketCount);
        m_VBOs         .resize(m_PacketCount);
        m_EBOs         .resize(m_PacketCount);

        m_IndexCounts  .resize(m_PacketCount);
        m_Transforms   .resize(m_PacketCount);

        m_MaterialTypes.resize(m_PacketCount);
        m_Materials    .resize(m_PacketCount);

        glGenVertexArrays(static_cast<GLsizei>(m_PacketCount), m_VAOs.data());
        glGenBuffers(     static_cast<GLsizei>(m_PacketCount), m_VBOs.data());
        glGenBuffers(     static_cast<GLsizei>(m_PacketCount), m_EBOs.data());

        for (size_t i = 0; i < m_PacketCount; ++i) {
            std::vector<Vertex> vertices = bundle.packets[i].vertexData;
            std::vector<Index> indices = bundle.packets[i].indexData;

            m_IndexCounts[i] = indices.size();
            m_Transforms[i] = bundle.transforms[i][0];

            m_MaterialTypes[i] = bundle.packets[i].materialType;
            m_Materials[i] = bundle.packets[i].material;

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

        m_Projection = glm::mat4{ 1.0f };
        m_Projection = glm::perspective(glm::radians(App::fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::nearPlane, App::farPlane);
    }
}