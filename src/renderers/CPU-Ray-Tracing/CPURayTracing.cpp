#include "CPURayTracing.h"
#include <iostream>

#include "renderers/OpenGl/utility/GLDebug.h"

#include "Settings/App.h"

namespace Rutile {
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
        // Positions              // Uvs
        -1.0f, -1.0f, 0.0f,       0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,       0.0f, 1.0f,
         1.0f,  1.0f, 0.0f,       1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };

    GLFWwindow* CPURayTracing::Init() {
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

    void CPURayTracing::Cleanup(GLFWwindow* window) {
        glDeleteTextures(1, &m_ScreenTexture);

        glfwDestroyWindow(window);
    }

    void CPURayTracing::Render() {
        std::vector<unsigned int> pixels;
        pixels.reserve(App::screenWidth * App::screenHeight);

        for (unsigned int x = 0; x < (unsigned int)App::screenWidth; ++x) {
            for (unsigned int y = 0; y < (unsigned int)App::screenHeight; ++y) {
                pixels.push_back(PixelShader(x, y));
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);

        glUseProgram(m_ShaderProgram);
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    unsigned CPURayTracing::PixelShader(unsigned x, unsigned y) {
        unsigned int val = 0;

        unsigned char* r = &((unsigned char*)&val)[0];
        unsigned char* g = &((unsigned char*)&val)[1];
        unsigned char* b = &((unsigned char*)&val)[2];
        unsigned char* a = &((unsigned char*)&val)[3];

        *r = 255;
        *g = 0;
        *b = 0;
        *a = 255;

        return val;
    }
}