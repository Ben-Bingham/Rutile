#include "CPURayTracing.h"
#include "imgui.h"
#include <iostream>

#include "renderers/OpenGl/utility/GLDebug.h"

#include "Settings/App.h"

#include "utility/Random.h"
#include "utility/ThreadPool.h"

namespace Rutile {
    float LinearToGamma(float component) {
        if (component > 0.0f) {
            return sqrt(component);
        }
        return 0.0f;
    }

    glm::vec3 LinearToGamma(glm::vec3 color) {
        return glm::vec3{ LinearToGamma(color.r), LinearToGamma(color.g), LinearToGamma(color.b) };
    }

    glm::vec4 RenderPixel(glm::u32vec2 pixelCoordinate) {
        glm::vec2 normalizedPixelCoordinate = { (float)pixelCoordinate.x / (float)App::screenWidth, (float)pixelCoordinate.y / (float)App::screenHeight };

        const float normalizedPixelWidth = 1.0f / (float)App::screenWidth;
        const float normalizedPixelHeight = 1.0f / (float)App::screenHeight;

        normalizedPixelCoordinate.x += normalizedPixelWidth / 2.0f;
        normalizedPixelCoordinate.y += normalizedPixelHeight / 2.0f;

        const float widthJitter = (RandomFloat() - 0.5f) * normalizedPixelWidth;
        const float heightJitter = (RandomFloat() - 0.5f) * normalizedPixelHeight;

        normalizedPixelCoordinate.x += widthJitter;
        normalizedPixelCoordinate.y += heightJitter;

        Ray ray;

        const glm::mat4 cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);

        const glm::mat4 inverseView = glm::inverse(App::camera.View());

        // This coordinate is the target of the ray, it starts in screen space, but this line brings it into clip space
        normalizedPixelCoordinate = normalizedPixelCoordinate * 2.0f - 1.0f; // Bring into the range [-1, 1]

        // Here we bring the target of the ray from clip space into view space
        const glm::vec4 target = inverseProjection * glm::vec4(normalizedPixelCoordinate.x, normalizedPixelCoordinate.y, 1, 1);

        // Finally we bring the ray target from view space into world space
        ray.direction = glm::vec3(inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));

        // The cameras position is already in world space, and so it does not need to be transformed
        ray.origin = App::camera.position;

        glm::vec3 pixelColor = FireRayIntoScene(ray);

        pixelColor = LinearToGamma(pixelColor);

        return glm::vec4{ pixelColor, 1.0f };
    }

    void RenderSection(Section* section) {
        int x = (int)section->startIndex % App::screenWidth;
        int y = (int)section->startIndex / App::screenWidth;

        for (size_t i = section->startIndex; i < section->startIndex + section->length; ++i) {
            section->pixels[i - section->startIndex] = RenderPixel(glm::u32vec2{ x, y });

            ++x;
            if (x == App::screenWidth) {
                x = 0;
                ++y;
            }
        }
    }

    glm::vec3 FireRayIntoScene(Ray ray) {
        constexpr float r = 1.0f; // Sphere radius in local space
        constexpr glm::vec3 spherePos = { 0.0f, 0.0f, 0.0f }; // Sphere position in local space

        float pixelColorMultiplier = 1.0f;

        for (int i = 0; i < App::settings.maxBounces; ++i) {
            bool hitSomething = false;
            float closestDistance = std::numeric_limits<float>::max();
            Object* hitObject = nullptr;
            glm::vec3 hitNormal{ };
            glm::vec3 hitPosition{ };

            for (auto& object : App::scene.objects) {
                const glm::mat4 invModel = glm::inverse(App::transformBank[object.transform].matrix);

                const glm::vec3 o = invModel * glm::vec4{ ray.origin, 1.0f };
                const glm::vec3 d = invModel * glm::vec4{ ray.direction, 0.0f };

                glm::vec3 co = spherePos - o;
                const float a = dot(d, d);
                const float b = -2.0f * glm::dot(d, co);
                const float c = dot(co, co) - (r * r);

                const float discriminant = (b * b) - (4.0f * a * c);

                if (discriminant < 0.0f) { // No intersection
                    continue;
                }

                const float sqrtDiscriminant = glm::sqrt(discriminant);

                // Because we subtract the discriminant, this root will always be smaller than the other one
                float t = (-b - sqrtDiscriminant) / (2.0f * a);

                if (t <= 0.001f || t >= std::numeric_limits<float>::max()) {
                    t = (-b + sqrtDiscriminant) / (2.0f * a);
                    if (t <= 0.001f || t >= std::numeric_limits<float>::max()) {
                        continue;
                    }
                }

                // At this point, no matter what t will be the closest hit for this object

                if (t < closestDistance) {
                    closestDistance = t;
                    hitSomething = true;
                    hitObject = &object;

                    glm::vec3 hitPointLocalSpace = o + t * d;

                    hitNormal = glm::normalize(hitPointLocalSpace - spherePos);

                    // Transform normal back to world space
                    glm::vec3 normalWorldSpace = glm::transpose(glm::inverse(glm::mat3(App::transformBank[object.transform].matrix))) * hitNormal;
                    hitNormal = glm::normalize(normalWorldSpace);

                    hitPosition = App::transformBank[object.transform].matrix * glm::vec4{ hitPointLocalSpace, 1.0f };
                }
            }

            if (hitSomething) {
                pixelColorMultiplier *= 0.5f;

                ray.origin = hitPosition;
                ray.direction = hitNormal + RandomUnitVec3();
            } else {
                break;
            }
        }

        return App::settings.backgroundColor * pixelColorMultiplier;
    }

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

        m_ThreadPool = std::make_unique<RayTracingThreadPool>(m_SectionCount);

        CalculateSections();

        ResetAccumulatedPixelData();

        return window;
    }

    void CPURayTracing::Cleanup(GLFWwindow* window) {
        m_ThreadPool.reset();

        glDeleteTextures(1, &m_ScreenTexture);

        glfwDestroyWindow(window);
    }

    void CPURayTracing::Render() {
        ++m_FrameCount;

        // Pixel Rendering
        const auto pixelRenderStart = std::chrono::steady_clock::now();
        for (auto& section : m_Sections) {
            section.pixels.clear();
            section.pixels.resize(section.length);

            m_ThreadPool->QueueJob(RenderSection, &section);
        }

        m_ThreadPool->WaitForCompletion();

        m_PixelRenderTime = std::chrono::steady_clock::now() - pixelRenderStart;

        // Section combining
        const auto sectionCombinationStart = std::chrono::steady_clock::now();

        std::vector<glm::vec4> combinedSectionData;
        combinedSectionData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);

        for (auto& section : m_Sections) {
            std::memcpy(combinedSectionData.data() + section.startIndex, section.pixels.data(), section.length * sizeof(glm::vec4));
        }

        std::vector<glm::vec4> imageData;
        imageData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);

        size_t i = 0;
        for (const auto& pixel : combinedSectionData) {
            m_AccumulatedPixelData[i] += pixel;

            imageData[i] = m_AccumulatedPixelData[i] / (float)m_FrameCount;
            ++i;
        }

        m_SectionCombinationTime = std::chrono::steady_clock::now() - sectionCombinationStart;

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

    void CPURayTracing::WindowResizeEvent() {
        CalculateSections();

        glViewport(0, 0, App::screenWidth, App::screenHeight);

        ResetAccumulatedPixelData();
    }

    void CPURayTracing::SignalNewScene() {
        ResetAccumulatedPixelData();
    }

    void CPURayTracing::CalculateSections() {
        m_Sections.clear();

        const size_t pixelCount = (size_t)App::screenWidth * (size_t)App::screenHeight;

        size_t remainder = pixelCount % m_SectionCount;
        size_t sectionSize = (pixelCount - remainder) / m_SectionCount;

        for (size_t i = 0; i < (size_t)m_SectionCount; ++i) {
            Section section{ };

            section.startIndex = i * sectionSize;
            section.length = sectionSize;

            m_Sections.push_back(section);
        }

        m_Sections.back().length += remainder;

        m_ThreadPool.reset();

        m_ThreadPool = std::make_unique<RayTracingThreadPool>(m_SectionCount);
    }

    void CPURayTracing::ProvideTimingStatistics() {
        ImGui::Separator();

        const auto totalPixelRenderTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_PixelRenderTime);
        ImGui::Text(("Total Pixel Rendering Time: " + std::to_string((double)totalPixelRenderTime.count() / 1000000.0) + "ms").c_str());

        const auto totalSectionCombinationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_SectionCombinationTime);
        ImGui::Text(("Total Section Combination Time: " + std::to_string((double)totalSectionCombinationTime.count() / 1000000.0) + "ms").c_str());

        ImGui::Separator();

        const auto averagePixelRenderTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_PixelRenderTime);
        ImGui::Text(("Average Pixel Rendering Time: " + std::to_string((double)averagePixelRenderTime.count() / 1000000.0 / (double)m_SectionCount) + "ms").c_str());

        const auto averageSectionCombinationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_SectionCombinationTime);
        ImGui::Text(("Average Section Combination Time: " + std::to_string((double)averageSectionCombinationTime.count() / 1000000.0 / (double)m_SectionCount) + "ms").c_str());
    }

    void CPURayTracing::ProvideLocalRendererSettings() {
        if (ImGui::DragInt("Section Count", &m_SectionCount, 0.01f, 1, 100)) {
            CalculateSections();
        }
    }

    void CPURayTracing::CameraUpdateEvent() {
        ResetAccumulatedPixelData();
    }

    void CPURayTracing::SignalObjectMaterialUpdate(ObjectIndex i) {
        ResetAccumulatedPixelData();
    }

    void CPURayTracing::SignalObjectTransformUpdate(ObjectIndex i) {
        ResetAccumulatedPixelData();
    }

    void CPURayTracing::ResetAccumulatedPixelData() {
        m_AccumulatedPixelData.clear();
        m_AccumulatedPixelData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);
        m_FrameCount = 0;
    }
}