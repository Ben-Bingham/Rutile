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

#include <implot.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "rendering/Camera.h"
#include "rendering/Material.h"

/*
 * TODO The current SetBundle (with opengl) will not work once I implement modifying materials and or transforms with ImGui, but
 * TODO if instead of storing copies of both materials and transforms, the renderers can either store pointers, or simply indices
 * TODO into a global list of materials/transforms, than if they are modified there is no need to recreate the whole bundle, instead
 * TODO the bundle only needs to be "re Set" when a new packet is added, or when the geometry of a packet is modified.
 *
 * TODO Instanced Rendering
 *
 * TODO Frame Time Stats
 */

GLFWwindow* window;

size_t width = 1200;
size_t height = 800;

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

int mouseX = 0;
int mouseY = 0;

void mouseMoveCallback(GLFWwindow* window, double x, double y) {
    mouseX = (int)x;
    mouseY = (int)y;
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
    glfwSetCursorPosCallback(window, mouseMoveCallback);

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

struct Settings {
    bool gpuVsync{ false };
    bool cpuVsync{ false };
};

Settings settings;

int main() {
    /* // TODO Implement this:
    Renderer rendereer;
    // In the init function for OpenGl a GLEW instance can be made
    // In the init function for Vulkan a Vulkan instance can be made
    // etc...
    GLFWwindow* window = renderer.init();

    renderer.setSize();
    renderer.setBundle();

    AddAllWindowThings(window); // Assign call backs, set up mouse movement, and key movement stuff

    while (window is open) {
        GUI(); // ImGui window

        if (lastRenderer != currentRenderer) {
            disconnectAllWindowThings();

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

            window = renderer->Init();

            addAllWindowThings();

            renderer->SetSize(width, height);
            renderer->SetBundle(bundle);
        }

        renderer->render();
    }
    */

    // Materials need to be reuploaded every frame
    // Transforms need to be reuploaded every frame
        // They only need to be recalculated when they change tho
    // Lights DO NOT need to be reuploaded every frame
        // But when something changes they do need to be


    // Renderer changes:
    // Add on a SetLight(size_t lightIndex, Light* newLight); function that sets the light at the specified index to the new light
        // This hopes that the renderer will be able to modify only one light at a time, if not a renderer could reset them all, and have
        // initialy saved a copy of the full bundle
    // Add on SetPacket(size_t packetIndex, const Packet& newPacket);  same as setlight but for packets

    // IF A LIGHT IS SET TO nullptr, IT IS CONSIDERD DELETED
    // IF A PACKET IS SET WITH NO VERTICES, IT IS CONSIDERD DELETED

    // More renderer changes:
    // Add a AddLight(Light* light); function that adds on a new light onto the end of the current list of lights;
    // Add a AddPacket(Packet) function that adds on a new packet to the end of the current list;

    /*    
    struct Packet {
        std::vector<vertices>
        std::vector<indices>

        Transform* transform

        Material* material
    };

    
    struct Bundle {
        std::vector<lights> lights;
        std::vector<bools> doesThisLightNeedToBeReuploaded; //DEFAULT TRUE, but set to false every time we do an upload

        std::vector<Renderable> renderables;
        std::vector<bool> doesThisRenderableNowHaveNewGeometry;
    };
     */

    GeometryPreprocessor geometryPreprocessor{ };

    Solid solid;
    solid.color = { 1.0f, 0.0f, 1.0f };

    Solid solid2;
    solid2.color = { 0.2f, 0.5f, 0.7f };

    Phong phong;
    phong.ambient = { 1.0f, 0.5f, 0.31f };
    phong.diffuse = { 1.0f, 0.5f, 0.31f };
    phong.specular = { 0.5f, 0.5f, 0.5f };
    phong.shininess = 32.0f;

    Phong phong2;
    //phong2.ambient

    glm::mat4 transform1 = glm::mat4{ 1.0f };
    transform1 = glm::translate(transform1, glm::vec3{ 1.0f, 1.0f, 0.0f });
    geometryPreprocessor.Add(Primitive::TRIANGLE, &transform1, MaterialType::SOLID, &solid);

    glm::mat4 transform2 = glm::mat4{ 1.0f };
    transform2 = glm::translate(transform2, glm::vec3{ -1.0f, -1.0f, 0.0f });
    geometryPreprocessor.Add(Primitive::TRIANGLE, &transform2, MaterialType::SOLID, &solid2);

    glm::mat4 transform3 = glm::mat4{ 1.0f };
    transform3 = glm::translate(transform3, glm::vec3{ 0.0f, 0.0f, 0.0f });
    geometryPreprocessor.Add(Primitive::SQUARE, &transform3, MaterialType::SOLID, &solid);

    glm::mat4 transform4 = glm::mat4{ 1.0f };
    transform4 = glm::translate(transform4, glm::vec3{ 1.0f, -1.0f, 0.0f });
    geometryPreprocessor.Add(Primitive::CUBE, &transform4, MaterialType::SOLID, &solid2);

    glm::mat4 transform5 = glm::mat4{ 1.0f };
    transform5 = glm::translate(transform5, glm::vec3{ -1.0f, 1.0f, 0.0f });
    geometryPreprocessor.Add(Primitive::CUBE, &transform5, MaterialType::PHONG, &phong);

    PointLight pointLight;
    pointLight.position = { -2.0f, 2.0f, 2.0f };

    pointLight.ambient = { 0.05f, 0.05f, 0.05f };
    pointLight.diffuse = { 0.8f, 0.8f, 0.8f };
    pointLight.specular = { 1.0f, 1.0f, 1.0f };

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    geometryPreprocessor.Add(LightType::POINT, &pointLight);

    DirectionalLight directionalLight;
    directionalLight.direction = { 0.0f, -1.0f, 0.0f };

    directionalLight.ambient = { 0.05f, 0.05f, 0.05f };
    directionalLight.diffuse = { 0.4f, 0.4f, 0.4f };
    directionalLight.specular = { 0.5f, 0.5f, 0.5f };

    geometryPreprocessor.Add(LightType::DIRECTION, &directionalLight);

    SpotLight spotLight;
    spotLight.position = { 0.0f, 0.0f, 0.0f };
    spotLight.direction = { 0.0f, 0.0f, -1.0f };

    spotLight.ambient = { 0.0f, 0.0f, 0.0f };
    spotLight.diffuse = { 1.0f, 1.0f, 1.0f };
    spotLight.specular = { 1.0f, 1.0f, 1.0f };

    spotLight.constant = 1.0f;
    spotLight.linear = 0.09f;
    spotLight.quadratic = 0.032f;

    spotLight.cutOff = glm::cos(glm::radians(12.5f));
    spotLight.outerCutOff = glm::cos(glm::radians(15.0f));

    geometryPreprocessor.Add(LightType::SPOTLIGHT, &spotLight);

    Bundle bundle = geometryPreprocessor.GetBundle(GeometryMode::OPTIMIZED);

    screenInit();

    std::unique_ptr<Renderer> renderer = std::make_unique<OpenGlRenderer>();
    renderer->Init();
    renderer->SetSize(width, height);
    renderer->SetBundle(bundle);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.FontGlobalScale = 2.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    std::chrono::duration<double> frameTime = std::chrono::duration<double>(1.0 / 60.0);

    Camera camera;

    bool mouseDown = false;

    int lastMouseX = 0;
    int lastMouseY = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        auto frameStart = std::chrono::system_clock::now();

        glfwPollEvents();

        float dt = static_cast<float>(frameTime.count());
        float velocity = camera.speed * dt;
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.position += camera.frontVector * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.position -= camera.frontVector * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.position += camera.rightVector * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.position -= camera.rightVector * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.position += camera.upVector * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.position -= camera.upVector * velocity;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            if (mouseDown == false) {
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }

            mouseDown = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            mouseDown = false;
        }

        if (mouseDown) {
            float xDelta = (float)mouseX - (float)lastMouseX;
            float yDelta = (float)lastMouseY - (float)mouseY; // reversed since y-coordinates go from bottom to top

            camera.yaw += xDelta * camera.lookSensitivity;
            camera.pitch += yDelta * camera.lookSensitivity;

            if (camera.pitch > 89.9f) {
                camera.pitch = 89.9f;
            } else if (camera.pitch < -89.9f) {
                camera.pitch = -89.9f;
            }

            camera.frontVector.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
            camera.frontVector.y = sin(glm::radians(camera.pitch));
            camera.frontVector.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
            camera.frontVector   = glm::normalize(camera.frontVector);

            camera.rightVector   = glm::normalize(glm::cross(camera.frontVector, camera.upVector));

            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }

        spotLight.position = camera.position;
        spotLight.direction = camera.frontVector;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        // GUI
        int lastRenderer = currentRenderer;
        { ImGui::Begin("Rutile");

            ImGui::Text("Renderer");

            ImGui::RadioButton("OpenGl",        &currentRenderer, OPENGL);          ImGui::SameLine();
            ImGui::RadioButton("Rainbow Time",  &currentRenderer, RAINBOW_TIME);    ImGui::SameLine();
            ImGui::RadioButton("Hard Coded",    &currentRenderer, HARD_CODED);

            ImGui::Text("Settings");
            ImGui::Checkbox("GPU Vsync", &settings.gpuVsync);
            ImGui::Checkbox("CPU Vsync", &settings.cpuVsync);

            if (ImGui::CollapsingHeader("Lights")) {
                int pointLightCount = 1;
                int directionalLightCount = 1;
                int spotLightCount = 1;
                int i = 0;
                for (auto lightType : bundle.lightTypes) {
                    switch (lightType) {
                        case LightType::POINT: {
                            PointLight* light = dynamic_cast<PointLight*>(bundle.lights[i]);
                            if (ImGui::TreeNode(("Point light #" + std::to_string(pointLightCount)).c_str())) {
                                ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(light->position), 0.05f);

                                if (ImGui::DragFloat(("Constant Attenuation Component##" + std::to_string(i)).c_str(), &light->constant, 0.005f, 0.0f, 1.0f)) {
                                    std::cout << "DRAGGING " << std::endl;
                                }
                                ImGui::DragFloat(("Linear Attenuation Component##" + std::to_string(i)).c_str(), &light->linear, 0.005f, 0.0f, 1.0f);
                                ImGui::DragFloat(("Quadratic Attenuation Component##" + std::to_string(i)).c_str(), &light->quadratic, 0.005f, 1.0f);

                                ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->ambient));
                                ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->diffuse));
                                ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->specular));

                                ImGui::TreePop();
                            }

                            ++pointLightCount;
                            break;
                        }
                        case LightType::DIRECTION: { // TODO
                            DirectionalLight* light = dynamic_cast<DirectionalLight*>(bundle.lights[i]);
                            if (ImGui::TreeNode(("Directional light #" + std::to_string(directionalLightCount)).c_str())) {
                                ImGui::DragFloat3(("Direction##" + std::to_string(i)).c_str(), glm::value_ptr(light->direction), 0.05f);

                                ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->ambient));
                                ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->diffuse));
                                ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->specular));

                                ImGui::TreePop();
                            }

                            ++directionalLightCount;
                            break;
                        }
                        case LightType::SPOTLIGHT: {
                            SpotLight* light = dynamic_cast<SpotLight*>(bundle.lights[i]);
                            if (ImGui::TreeNode(("Spotlight #" + std::to_string(spotLightCount)).c_str())) {
                                ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(light->position), 0.05f);
                                ImGui::DragFloat3(("Direction##" + std::to_string(i)).c_str(), glm::value_ptr(light->direction), 0.05f);

                                float cutOff = glm::degrees(glm::acos(light->cutOff));
                                float outerCutOff = glm::degrees(glm::acos(light->outerCutOff));

                                ImGui::DragFloat(("Inner Cut Off##" + std::to_string(i)).c_str(), &cutOff, 0.5f, 0.0f, 180.0f);
                                ImGui::DragFloat(("Outer Cut Off##" + std::to_string(i)).c_str(), &outerCutOff, 0.5f, 0.0f, 180.0f);

                                spotLight.cutOff = glm::cos(glm::radians(cutOff));
                                spotLight.outerCutOff = glm::cos(glm::radians(outerCutOff));

                                ImGui::DragFloat(("Constant Attenuation Component##" + std::to_string(i)).c_str(), &light->constant, 0.005f, 0.0f, 1.0f);
                                ImGui::DragFloat(("Linear Attenuation Component##" + std::to_string(i)).c_str(), &light->linear, 0.005f, 0.0f, 1.0f);
                                ImGui::DragFloat(("Quadratic Attenuation Component##" + std::to_string(i)).c_str(), &light->quadratic, 0.005f, 1.0f);

                                ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->ambient));
                                ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->diffuse));
                                ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(light->specular));

                                ImGui::TreePop();
                            }
                            ++spotLightCount;
                            break;
                        }
                    }
                    ++i;
                }
            }

            if (ImGui::CollapsingHeader("Materials")) {
                std::vector<std::pair<MaterialType, Material*>> materials;
                for (auto packet : bundle.packets) {
                    MaterialType type = packet.materialType;
                    Material* material = packet.material;

                    bool alreadyHave = false;
                    for (auto mat : materials) {
                        if (mat.first == type && mat.second == material) {
                            alreadyHave = true;
                            break;
                        }
                    }

                    if (!alreadyHave) {
                        materials.push_back(std::make_pair(type, material));
                    }
                }

                int solidMaterialCount = 1;
                int phongMaterialCount = 1;
                int i = 0;
                for (auto mat : materials) {
                    switch (mat.first) {
                        case MaterialType::SOLID: {
                            Solid* material = dynamic_cast<Solid*>(mat.second);
                            if (ImGui::TreeNode(("Solid Material #" + std::to_string(solidMaterialCount)).c_str())) {
                                ImGui::ColorEdit3(("Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->color));

                                ImGui::TreePop();
                            }
                            ++solidMaterialCount;
                            break;
                        }
                        case MaterialType::PHONG: {
                            Phong* material = dynamic_cast<Phong*>(mat.second);
                            if (ImGui::TreeNode(("Phong Material #" + std::to_string(phongMaterialCount)).c_str())) {
                                ImGui::ColorEdit3(("Ambient Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->ambient));
                                ImGui::ColorEdit3(("Diffuse Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->diffuse));
                                ImGui::ColorEdit3(("Specular Color##" + std::to_string(i)).c_str(), glm::value_ptr(material->specular));

                                ImGui::DragFloat(("Shininess##" + std::to_string(i)).c_str(), &material->shininess, 0.5f, 0.0f, 10000.0f);

                                ImGui::TreePop();
                            }
                            ++phongMaterialCount;
                            break;
                        }
                    }
                    ++i;
                }
            }

            if (ImGui::CollapsingHeader("Transforms")) {
                int transformCount = 1;
                int i = 0;
                for (auto t : bundle.transforms) {
                    glm::mat4* transform = t[0];

                    if (ImGui::TreeNode(("Transform #" + std::to_string(transformCount)).c_str())) {
                        glm::vec3 translation = { (*transform)[3][0], (*transform)[3][1], (*transform)[3][2] };
                        glm::vec3 translationBackup = translation;
                        ImGui::DragFloat3(("Translation##" + std::to_string(i)).c_str(), glm::value_ptr(translation), 0.01f);
                        *transform = glm::translate(*transform, translation - translationBackup);

                        glm::vec3 scaling = { (*transform)[0][0], (*transform)[1][1], (*transform)[2][2] };
                        glm::vec3 scalingBackup = scaling - 1.0f;
                        ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(), glm::value_ptr(scaling), 0.01f, 0.1f, 10000.0f);
                        *transform = glm::scale(*transform, scaling - scalingBackup);

                        ImGui::TreePop();
                    }
                    ++transformCount;
                    ++i;
                }
            }

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

            renderer->Init();
            renderer->SetSize(width, height);
            renderer->SetBundle(bundle);
        }

        if (resize) {
            renderer->SetSize(width, height);
            resize = false;
        }

        if (settings.gpuVsync) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }

        // Rendering
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
        std::vector<Pixel> pixels = renderer->Render(camera, projection);

        // Rendering texture with pixel data
        if (!pixels.empty()) {
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

        auto frameEnd = std::chrono::system_clock::now();

        frameTime = frameEnd - frameStart;

        if (settings.cpuVsync) {
            auto elapsedTime = frameEnd - frameStart;

            auto frameDuration = std::chrono::duration<double>(1.0 / 60.0);

            auto elapsedTimeSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);

            if (elapsedTimeSeconds < frameDuration) {
                auto timeToWait = frameDuration - elapsedTimeSeconds;
                std::this_thread::sleep_for(timeToWait);
            }

            frameTime = std::chrono::system_clock::now() - frameStart;
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
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