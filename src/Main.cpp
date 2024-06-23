#include <iostream>

#include "GeometryPreprocessor.h"
#include "renderers/renderer.h"
#include "renderers/OpenGl/OpenGlRenderer.h"

#include "renderers/HardCoded/HardCodedRenderer.h"
#include "renderers/RainbowTime/RainbowTimeRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

GLFWwindow* window;

size_t width = 600;
size_t height = 400;

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
    "uniform sampler2D tex;\n"
    "\n"
    "void main() {\n"
    "   outFragColor = texture(tex, uv);\n"
    "}\n\0";

unsigned int VAO;
unsigned int VBO;
unsigned int EBO;

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

void glfwErrorCallback(int error, const char* description) {
    std::cout << "ERROR: GLFW has thrown an error: " << std::endl;
    std::cout << description << std::endl;
}

bool resize = false;

void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
    width = static_cast<size_t>(w);
    height = static_cast<size_t>(h);
    glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

    resize = true;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam);

void screenInit() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
    }

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "Rutile", nullptr, nullptr);

    if (!window) {
        std::cout << "ERROR: Failed to create window." << std::endl;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

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
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
}

void screenCleanup() {
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}

using namespace Rutile;

enum RendererEnum {
    OPENGL          = 1,
    HARD_CODED      = 2,
    RAINBOW_TIME    = 3
};

int currentRenderer = OPENGL;

int main() {
    GeometryPreprocessor geometryPreprocessor{ };

    screenInit();

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();
    renderer->Init(width, height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Renderer Switching
        int lastRenderer = currentRenderer;
        { ImGui::Begin("Rutile");

            ImGui::Text("Renderer");

            ImGui::RadioButton("OpenGl",        &currentRenderer, OPENGL);          ImGui::SameLine();
            ImGui::RadioButton("Rainbow Time",  &currentRenderer, RAINBOW_TIME);    ImGui::SameLine();
            ImGui::RadioButton("Hard Coded",    &currentRenderer, HARD_CODED);
            
        } ImGui::End();

        if (lastRenderer != currentRenderer) {
            renderer->Cleanup();

            renderer.reset();

            switch (currentRenderer) {
            case OPENGL:
                renderer = std::make_unique<OpenGlRenderer>();
                break;

            case HARD_CODED:
                renderer = std::make_unique<HardCodedRenderer>();
                break;

            case RAINBOW_TIME:
                renderer = std::make_unique<RainbowTimeRenderer>();
                break;
            }

            renderer->Init(width, height);
        }

        if (resize) {
            renderer->Resize(width, height);
            resize = false;
        }

        // Rendering
        glm::mat4 transform = glm::mat4{ 1.0f };
        transform = glm::translate(transform, glm::vec3{ 1.0f, 1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, transform);

        transform = glm::mat4{ 1.0f };
        transform = glm::translate(transform, glm::vec3{ -1.0f, -1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, transform);

        transform = glm::mat4{ 1.0f };
        transform = glm::translate(transform, glm::vec3{ 0.0f, 0.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::TRIANGLE, transform);

        transform = glm::mat4{ 1.0f };
        transform = glm::translate(transform, glm::vec3{ 1.0f, -1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::CUBE, transform);

        transform = glm::mat4{ 1.0f };
        transform = glm::translate(transform, glm::vec3{ -1.0f, 1.0f, 0.0f });
        geometryPreprocessor.Add(Primitive::CUBE, transform);

        Bundle bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

        glm::vec3 cameraPos     { 0.0f,  0.0f,  7.5f };
        glm::vec3 cameraFront   { 0.0f,  0.0f, -1.0f };
        glm::vec3 cameraUp      { 0.0f,  1.0f,  0.0f };

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);

        std::vector<Pixel> pixels = renderer->Render(bundle, view, projection);

        // Rendering texture with pixel data
        {
            unsigned int texture;

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            glGenerateMipmap(GL_TEXTURE_2D);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

            glDeleteTextures(1, &texture);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* currentContextBackup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(currentContextBackup);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    renderer->Cleanup();

    screenCleanup();
}

void APIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam
) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}