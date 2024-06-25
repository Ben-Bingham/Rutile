#include "OpenGlRenderer.h"
#include "imgui.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

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

    void OpenGlRenderer::Init(size_t width, size_t height) {
        m_Width = width;
        m_Height = height;

        m_SolidShader = createShader("assets\\shaders\\renderers\\OpenGl\\solid.vert", "assets\\shaders\\renderers\\OpenGl\\solid.frag");
        m_PhongShader = createShader("assets\\shaders\\renderers\\OpenGl\\phong.vert", "assets\\shaders\\renderers\\OpenGl\\phong.frag");

        // Framebuffer creation
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        glGenTextures(1, &m_FBOTexture);
        glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOTexture, 0);

        glGenRenderbuffers(1, &m_RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Framebuffer is not complete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    std::vector<Pixel> OpenGlRenderer::Render(const Bundle& bundle, const Camera& camera, const glm::mat4& projection) {
        GLFWwindow* currentContextBackup = glfwGetCurrentContext();

        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::vector<unsigned int> VAOs;
        std::vector<unsigned int> VBOs;
        std::vector<unsigned int> EBOs;

        VAOs.resize(bundle.packets.size());
        VBOs.resize(bundle.packets.size());
        EBOs.resize(bundle.packets.size());

        glGenVertexArrays(static_cast<GLsizei>(bundle.packets.size()), VAOs.data());
        glGenBuffers     (static_cast<GLsizei>(bundle.packets.size()), VBOs.data());
        glGenBuffers     (static_cast<GLsizei>(bundle.packets.size()), EBOs.data());

        for (size_t i = 0; i < bundle.packets.size(); ++i) {
            std::vector<Vertex> vertices = bundle.packets[i].vertexData;
            std::vector<Index> indices = bundle.packets[i].indexData;

            MaterialType type = bundle.packets[i].materialType;
            Material* material = bundle.packets[i].material;

            unsigned int* shaderProgram = nullptr;

            switch (type) {
                case MaterialType::SOLID: {
                    Solid* solid = dynamic_cast<Solid*>(material);

                    shaderProgram = &m_SolidShader;
                    glUseProgram(m_SolidShader);

                    glUniform3fv(glGetUniformLocation(m_SolidShader, "color"), 1, glm::value_ptr(solid->color));
                    break;
                }
                case MaterialType::PHONG: {
                    Phong* phong = dynamic_cast<Phong*>(material);

                    shaderProgram = &m_PhongShader;
                    glUseProgram(m_PhongShader);

                    glUniform3fv(glGetUniformLocation(m_PhongShader, "phong.ambient"), 1, glm::value_ptr(phong->ambient));
                    glUniform3fv(glGetUniformLocation(m_PhongShader, "phong.diffuse"), 1, glm::value_ptr(phong->diffuse));
                    glUniform3fv(glGetUniformLocation(m_PhongShader, "phong.specular"), 1, glm::value_ptr(phong->ambient));
                    glUniform1f(glGetUniformLocation(m_PhongShader, "phong.shininess"), phong->shininess);

                    glUniformMatrix4fv(glGetUniformLocation(*shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(bundle.transforms[i][0]));

                    glUniform3fv(glGetUniformLocation(m_PhongShader, "cameraPosition"), 1, glm::value_ptr(camera.position));
                    break;
                }
            }

            for (auto transform : bundle.transforms[i]) {
                // TODO Each transfrom is for a different instance of the same vertex/index data
            }

            glBindVertexArray(VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
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

            glm::mat4 mvp = projection * camera.View() * bundle.transforms[i][0];

            glUniformMatrix4fv(glGetUniformLocation(*shaderProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, (int)bundle.packets[i].indexData.size(), GL_UNSIGNED_INT, nullptr);
        }

        glDeleteBuffers(static_cast<GLsizei>(bundle.packets.size()), EBOs.data());
        glDeleteBuffers(static_cast<GLsizei>(bundle.packets.size()), VBOs.data());
        glDeleteVertexArrays(static_cast<GLsizei>(bundle.packets.size()), VAOs.data());

        glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
        std::vector<Pixel> pixels{ };
        pixels.resize(m_Width * m_Height);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels.data());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_DEPTH_TEST);

        glfwMakeContextCurrent(currentContextBackup);

        return pixels;
    }

    void OpenGlRenderer::Resize(size_t width, size_t height) {
        m_Width = width;
        m_Height = height;

        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOTexture, 0);
        
        glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void OpenGlRenderer::Cleanup() {
        glDeleteRenderbuffers(1, &m_RBO);
        glDeleteTextures(1, &m_FBOTexture);
        glDeleteFramebuffers(1, &m_FBO);

        glDeleteProgram(m_PhongShader);
        glDeleteProgram(m_SolidShader);
    }
}