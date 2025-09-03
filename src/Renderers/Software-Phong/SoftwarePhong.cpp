#include "SoftwarePhong.h"

#include <iostream>

#include "Settings/App.h"
#include "Utility/OpenGl/GLDebug.h"

namespace Rutile {
    GLFWwindow* SoftwarePhong::Init() {
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

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "ERROR: Vertex shader failed to compile:" << std::endl;
            std::cout << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cout << "ERROR: Fragment shader failed to compile:" << std::endl;
            std::cout << infoLog << std::endl;
        }

        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vertexShader);
        glAttachShader(m_ShaderProgram, fragmentShader);
        glLinkProgram(m_ShaderProgram);

        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, infoLog);
            std::cout << "ERROR: Shader program failed to link:" << std::endl;
            std::cout << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glUseProgram(m_ShaderProgram);
        glUniform1i(glGetUniformLocation(m_ShaderProgram, "tex"), 0);

        glGenTextures(1, &m_ScreenTexture);
        glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return window;

    }

    void SoftwarePhong::Cleanup(GLFWwindow* window) {
        glDeleteTextures(1, &m_ScreenTexture);

        glfwDestroyWindow(window);
    }

    void SoftwarePhong::Render() {
        std::vector<glm::vec4> imageData;
        imageData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);

        using DepthType = float;
        std::vector<DepthType> depthBuffer;
        depthBuffer.resize((size_t)App::screenWidth * (size_t)App::screenHeight);

        // For each triangle in the scene:
            // Transform it into screen space
            // Find what pixels it occupys
            // Find the triangles depth at each pixels -> depth buffer
        // For each pixel compare it to the depth buffer, for each pixel that is closest run the fragment shader

        // Bottom left of screen is (0, 0) and top right is (1, 1)
        const float pixelWidth = 1.0f / (float)App::screenWidth;
        const float pixelHeight = 1.0f / (float)App::screenHeight;

        glm::vec3 min{ std::numeric_limits<float>::max() };
        glm::vec3 max{ -std::numeric_limits<float>::max() };

        // Iterates over all objects
        for (auto& obj : App::scene.objects) {
            Geometry& geo = App::scene.geometryBank[obj.geometry];
            auto& vert = geo.vertices;
            auto& inds = geo.indices;

            Transform& transform = App::scene.transformBank[obj.transform];
            transform.CalculateMatrix(); // TODO super ineficient

            // TODO super ineficient
            glm::mat4 proj = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);

            glm::mat4 mvp = proj * App::camera.View() * transform.matrix;

            // Iterates over all triangles
            for (size_t i = 0; i < inds.size(); i += 3) {
                Vertex v1 = vert[inds[i + 0]];
                Vertex v2 = vert[inds[i + 1]];
                Vertex v3 = vert[inds[i + 2]];

                v1.position = glm::vec3{ mvp * glm::vec4{ v1.position.x, v1.position.y, v1.position.z, 1.0 } };
                v2.position = glm::vec3{ mvp * glm::vec4{ v2.position.x, v2.position.y, v2.position.z, 1.0 } };
                v3.position = glm::vec3{ mvp * glm::vec4{ v3.position.x, v3.position.y, v3.position.z, 1.0 } };

                min = glm::min(glm::min(glm::min(v1.position, v2.position), v3.position), min);
                max = glm::max(glm::max(glm::max(v1.position, v2.position), v3.position), max);

                glm::ivec2 p1 = glm::ivec2{ (int)std::floor(v1.position.x / pixelWidth), (int)std::floor(v1.position.y / pixelHeight) };
                glm::ivec2 p2 = glm::ivec2{ (int)std::floor(v2.position.x / pixelWidth), (int)std::floor(v2.position.y / pixelHeight) };
                glm::ivec2 p3 = glm::ivec2{ (int)std::floor(v3.position.x / pixelWidth), (int)std::floor(v3.position.y / pixelHeight) };

                //glm::ivec2 min{ App::screenWidth, App::screenHeight };
                //glm::ivec2 max{ 0, 0 };

                //if (p1.x > max.x) max.x = p1.x;
                //if (p2.x > max.x) max.x = p2.x;
                //if (p3.x > max.x) max.x = p3.x;

                //if (p1.y > max.y) max.y = p1.y;
                //if (p2.y > max.y) max.y = p2.y;
                //if (p3.y > max.y) max.y = p3.y;

                //if (p1.x < min.x) min.x = p1.x;
                //if (p2.x < min.x) min.x = p2.x;
                //if (p3.x < min.x) min.x = p3.x;

                //if (p1.y < min.y) min.y = p1.y;
                //if (p2.y < min.y) min.y = p2.y;
                //if (p3.y < min.y) min.y = p3.y;

                //for (int x = min.x; x < max.x; ++x) {
                //    for (int y = min.y; y < max.y; ++y) {
                //        depthBuffer[x + y * App::screenWidth] = 1.0;
                //    }
                //}
            }
        }

        std::cout << "min: (" << min.x << ", " << min.y << ", " << min.z << ")" << std::endl;
        std::cout << "max: (" << max.x << ", " << max.y << ", " << max.z << ")" << std::endl;

        for (int y = 0; y < App::screenHeight; ++y) {
            for (int x = 0; x < App::screenWidth; ++x) {
                imageData[x + y * App::screenWidth] = glm::vec4{ depthBuffer[x + y * App::screenWidth], depthBuffer[x + y * App::screenWidth], depthBuffer[x + y * App::screenWidth], 1.0 };
                imageData[x + y * App::screenWidth] = glm::vec4{ (float)y / (float)App::screenHeight, 0.0, 0.0, 1.0 };
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, imageData.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);

        glUseProgram(m_ShaderProgram);
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    void SoftwarePhong::Notify(Event* event) {

    }

    void SoftwarePhong::LoadScene() {

    }
}