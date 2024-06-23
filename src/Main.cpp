#include <iostream>

#include "GeometryPreprocessor.h"
#include "renderers/HardCodedRenderer.h"
#include "renderers/renderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* window;

constexpr size_t width = 600;
constexpr size_t height = 400;

unsigned int shaderProgram;

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
    "void main() {\n"
    "   outFragColor = vec4(uv.x, uv.y, 0.0f, 1.0f);\n"
    "}\n\0";

unsigned int VAO;
unsigned int VBO;

std::vector vertices = {
    -0.5f, 0.0f, 0.0f,      0.0f, 0.0f,
     0.0f, 0.5f, 0.0f,      0.5f, 1.0f,
     0.5f, 0.0f, 0.0f,      1.0f, 0.0f
};

unsigned int texture;

void screenInit() {
    if (!glfwInit()) {
        std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
    }

    window = glfwCreateWindow(width, height, "Rutile", nullptr, nullptr);

    if (!window) {
        std::cout << "ERROR: Failed to create window." << std::endl;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
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

    shaderProgram = glCreateProgram();
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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void screenCleanup() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}

using namespace Rutile;

int main() {
    GeometryPreprocessor geometryPreprocessor{ };
    // geometryPreprocessor.add();

    screenInit();

    std::unique_ptr<Renderer> renderer = std::make_unique<HardCodedRenderer>();
    renderer->Init();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        Bundle bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

        std::vector<Pixel> pixels = renderer->Render(bundle, width, height);

        // Present to screen

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    renderer->Cleanup();

    screenCleanup();
}