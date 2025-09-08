#include "CPURayTracing.h"

#include <iostream>

#include "imgui.h"

#include "Utility/Random.h"
#include "Utility/ThreadPool.h"
#include "Utility/OpenGl/GLDebug.h"

#include <glm/ext/matrix_clip_space.hpp>

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

    glm::vec4 RenderPixel(
        glm::vec2 normalizedPixelCoordinate,
        const Camera& camera, 
        Scene& scene,
        const glm::vec2& normalizedPixelSize,
        const glm::mat4& inverseCameraProjection,
        const glm::mat4& inverseView
    ) {
        normalizedPixelCoordinate.x += normalizedPixelSize.x / 2.0f;
        normalizedPixelCoordinate.y += normalizedPixelSize.y / 2.0f;

        const float widthJitter = (RandomFloat() - 0.5f) * normalizedPixelSize.x;
        const float heightJitter = (RandomFloat() - 0.5f) * normalizedPixelSize.y;

        normalizedPixelCoordinate.x += widthJitter;
        normalizedPixelCoordinate.y += heightJitter;


        // This coordinate is the target of the ray, it starts in screen space, but this line brings it into clip space
        normalizedPixelCoordinate = normalizedPixelCoordinate * 2.0f - 1.0f; // Bring into the range [-1, 1]

        // Here we bring the target of the ray from clip space into view space
        const glm::vec4 target = inverseCameraProjection * glm::vec4(normalizedPixelCoordinate.x, normalizedPixelCoordinate.y, 1, 1);

        Ray ray;

        // Finally we bring the ray target from view space into world space
        ray.direction = glm::normalize(glm::vec3{ inverseView * glm::vec4{ glm::normalize(glm::vec3{ target } / target.w), 0 } });

        // The cameras position is already in world space, and so it does not need to be transformed
        ray.origin = camera.position;

        glm::vec3 pixelColor = FireRayIntoScene(ray, scene);

        pixelColor = LinearToGamma(pixelColor);

        return glm::vec4{ pixelColor, 1.0f };
    }

    void RenderSection(Section* section) {
        int x = (int)section->startIndex % section->screenSize.x;
        int y = (int)section->startIndex / section->screenSize.x;

        const glm::vec2 normalizedPixelSize{ 1.0f / (float)section->screenSize.x, 1.0f / (float)section->screenSize.y };

        const glm::mat4 cameraProjection = glm::perspective(glm::radians(section->camera.fov), (float)section->screenSize.x / (float)section->screenSize.y, section->camera.nearPlane, section->camera.farPlane);
        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);

        const glm::mat4 inverseView = glm::inverse(section->camera.View());

        for (size_t i = section->startIndex; i < section->startIndex + section->length; ++i) {
            glm::vec2 normalizedPixelCoordinate = { (float)x / (float)section->screenSize.x, (float)y / (float)section->screenSize.y };

            section->pixels[i - section->startIndex] = RenderPixel(
                normalizedPixelCoordinate,
                section->camera, 
                section->scene, 
                normalizedPixelSize,
                inverseProjection,
                inverseView
            );

            ++x;
            if (x == section->screenSize.x) {
                x = 0;
                ++y;
            }
        }
    }

    glm::vec3 FireRayIntoScene(Ray ray, Scene scene) {
        constexpr float r = 1.0f; // Sphere radius in local space
        constexpr glm::vec3 spherePos = { 0.0f, 0.0f, 0.0f }; // Sphere position in local space

        for (auto& object : scene.objects) {
            const glm::mat4 invModel = glm::inverse(object.transform);

            const glm::vec3 o = invModel * glm::vec4{ ray.origin, 1.0f };

            glm::vec3 d = invModel * glm::vec4{ ray.direction, 0.0f }; // TODO pick one
            //glm::vec3 d = glm::transpose(inverse(glm::mat3(invModel))) * ray.direction;
            d = normalize(d);

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

            glm::vec3 hitPointWorldSpace = object.transform * glm::vec4{ o + t * normalize(d), 1.0 };

            float lengthAlongRayWorldSpace = length(hitPointWorldSpace - ray.origin);

            if (lengthAlongRayWorldSpace < std::numeric_limits<float>::max()) {
                return object.material->diffuse;
            }
        }

        return CPURayTracing::backgroundColour;
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

    //void CPURayTracing::Notify(Event* event) {
    //    if (EVENT_IS(event, WindowResize)) {
    //        CalculateSections();

    //        glViewport(0, 0, App::screenWidth, App::screenHeight);

    //        ResetAccumulatedPixelData();
    //    }
    //    if (EVENT_IS(event, CameraUpdate)          || 
    //        EVENT_IS(event, ObjectTransformUpdate) || 
    //        EVENT_IS(event, ObjectMaterialUpdate)) {

    //        ResetAccumulatedPixelData();
    //    }
    //}

    CPURayTracing::CPURayTracing() {
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
    }

    CPURayTracing::~CPURayTracing() {
        m_ThreadPool.reset();

        glDeleteTextures(1, &m_ScreenTexture);
    }

    void CPURayTracing::Render(RenderTarget& target, const Camera& camera) {
        ++m_FrameCount;

        if (m_SectionCountChange) {
            CalculateSections(target.GetSize());
            m_SectionCountChange = false;
        }

        if (m_ResetAccumulatedPixelData) {
            ResetAccumulatedPixelData(target.GetSize());
            m_ResetAccumulatedPixelData = false;
        }

        // Pixel Rendering
        const auto pixelRenderStart = std::chrono::steady_clock::now();
        for (auto& section : m_Sections) {
            section.pixels.clear();
            section.pixels.resize(section.length);
            section.screenSize = target.GetSize();
            section.camera = camera;
            section.scene = m_Scene;

            m_ThreadPool->QueueJob(RenderSection, &section);
        }

        m_ThreadPool->WaitForCompletion();
        m_PixelRenderTime = std::chrono::steady_clock::now() - pixelRenderStart;

        // Section combining
        const auto sectionCombinationStart = std::chrono::steady_clock::now();

        std::vector<glm::vec4> combinedSectionData;
        combinedSectionData.resize((size_t)target.GetSize().x * (size_t)target.GetSize().y);

        for (auto& section : m_Sections) {
            std::memcpy(combinedSectionData.data() + section.startIndex, section.pixels.data(), section.length * sizeof(glm::vec4));
        }

        std::vector<glm::vec4> imageData;
        imageData.resize((size_t)target.GetSize().x * (size_t)target.GetSize().y);

        size_t i = 0;
        for (const auto& pixel : combinedSectionData) {
            m_AccumulatedPixelData[i] += pixel;

            imageData[i] = m_AccumulatedPixelData[i] / (float)m_FrameCount;
            ++i;
        }

        m_SectionCombinationTime = std::chrono::steady_clock::now() - sectionCombinationStart;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, target.GetSize().x, target.GetSize().y, 0, GL_RGBA, GL_FLOAT, imageData.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ScreenTexture);

        target.Bind();

        glUseProgram(m_ShaderProgram);
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);

        target.Unbind();
    }

    void CPURayTracing::SetScene(Scene scene) {
        m_Scene = scene;

        m_ResetAccumulatedPixelData = true;
    }

    void CPURayTracing::ProvideGeneralGUI() {
        if (ImGui::DragInt("Section Count", &m_SectionCount, 0.01f, 1, 100)) {
            m_SectionCountChange = true;
        }

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

    void CPURayTracing::UpdateObjectTransform(size_t i, const glm::mat4& newTransform) {
        m_ResetAccumulatedPixelData = true;
    }

    void CPURayTracing::UpdateObjectMaterial(size_t i, const std::shared_ptr<Material> newMaterial) {
        m_ResetAccumulatedPixelData = true;
    }

    void CPURayTracing::CalculateSections(glm::ivec2 screenSize) {
        m_Sections.clear();

        const size_t pixelCount = (size_t)screenSize.x * (size_t)screenSize.y;

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

    void CPURayTracing::ResetAccumulatedPixelData(glm::ivec2 screenSize) {
        m_AccumulatedPixelData.clear();
        m_AccumulatedPixelData.resize((size_t)screenSize.x * (size_t)screenSize.y);
        m_FrameCount = 0;
    }
}