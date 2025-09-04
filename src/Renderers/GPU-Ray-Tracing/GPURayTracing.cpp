//#include "GPURayTracing.h"
//#include "imgui.h"
//#include <iostream>
//
//#define GLM_ENABLE_EXPERIMENTAL
//#include <GLFW/glfw3native.h>
//
//#include <glm/gtx/string_cast.hpp>
//
//#include "GUI/ImGuiUtil.h"
//
//#include "Settings/App.h"
//
//#include "Utility/GeometryFactory.h"
//#include "Utility/events/Events.h"
//#include "Utility/OpenGl/GLDebug.h"
//#include "Utility/RayTracing/BoundingVolumeHierarchy/BVHBank.h"
//#include "Utility/RayTracing/BoundingVolumeHierarchy/BVHFactory.h"
//
//namespace Rutile {
//    GLFWwindow* GPURayTracing::Init() {
//        m_RendererLoadTime = std::chrono::steady_clock::now();
//
//        std::vector<float> vertices = {
//            // Positions
//            -1.0f, -1.0f, 0.0f,
//            -1.0f,  1.0f, 0.0f,
//             1.0f,  1.0f, 0.0f,
//             1.0f, -1.0f, 0.0f,
//        };
//
//        std::vector<unsigned int> indices = {
//            0, 1, 2,
//            0, 2, 3
//        };
//
//        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
//        GLFWwindow* window = glfwCreateWindow(App::screenWidth, App::screenHeight, App::name.c_str(), nullptr, nullptr);
//        glfwShowWindow(window);
//
//        if (!window) {
//            std::cout << "ERROR: Failed to create window." << std::endl;
//        }
//
//        glfwMakeContextCurrent(window);
//
//        if (glewInit() != GLEW_OK) {
//            std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
//        }
//
//        int flags;
//        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
//        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
//            glEnable(GL_DEBUG_OUTPUT);
//            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//            glDebugMessageCallback(glDebugOutput, nullptr);
//            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
//        }
//
//        m_RayTracingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\GPURayTracing\\GPURayTracing.vert", "assets\\shaders\\renderers\\GPURayTracing\\GPURayTracing.frag");
//        m_RenderingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\GPURayTracing\\Rendering.vert", "assets\\shaders\\renderers\\GPURayTracing\\Rendering.frag");
//
//        // Screen Rectangle
//        glGenVertexArrays(1, &m_VAO);
//        glGenBuffers(1, &m_VBO);
//        glGenBuffers(1, &m_EBO);
//
//        glBindVertexArray(m_VAO);
//
//        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
//
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
//        glEnableVertexAttribArray(0);
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//        // Accumulation Framebuffer
//        glGenFramebuffers(1, &m_AccumulationFrameBuffer);
//        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
//
//        glGenTextures(1, &m_AccumulationTexture);
//        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
//
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);
//
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//        glGenRenderbuffers(1, &m_AccumulationRBO);
//        glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);
//
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);
//
//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//            std::cout << "ERROR: Accumulation Framebuffer is not complete" << std::endl;
//        }
//
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        m_RayTracingShader->Bind();
//        m_RayTracingShader->SetInt("maxBounces", App::settings.maxBounces);
//
//        m_MaterialBank = std::make_unique<SSBO<LocalMaterial>>(0);
//        m_ObjectBank = std::make_unique<SSBO<LocalObject>>(1);
//        m_MeshBank = std::make_unique<SSBO<float>>(2);
//        m_TLASBank = std::make_unique<SSBO<LocalTLASNode>>(3);
//        m_BLASBank = std::make_unique<SSBO<LocalBLASNode>>(4);
//
//        ResetAccumulatedPixelData();
//        return window;
//    }
//
//    void GPURayTracing::Notify(Event* event) {
//        if (EVENT_IS(event, ObjectTransformUpdate)) {
//            CreateAndUploadBVHAndMeshAndObjectBuffers(); // TODO we really only need to change the object transforms,
//            // TODO but because the objects need starting indices, and we do all of it at once, we just call this
//        }
//        if (EVENT_IS(event, ObjectMaterialUpdate)) {
//            CreateAndUploadMaterialBuffer();
//        }
//        if (EVENT_IS(event, WindowResize)) {
//            glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
//            glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
//
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
//
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);
//
//            glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);
//
//            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
//            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);
//
//            ResetAccumulatedPixelData();
//        }
//        if (EVENT_IS(event, CameraUpdate)          || 
//            EVENT_IS(event, ObjectTransformUpdate) || 
//            EVENT_IS(event, ObjectMaterialUpdate)) {
//
//            ResetAccumulatedPixelData();
//        }
//    }
//
//    void GPURayTracing::ProjectionMatrixUpdate() {
//        ResetAccumulatedPixelData();
//    }
//
//    void GPURayTracing::Cleanup(GLFWwindow* window) {
//        m_BLASBank.reset();
//        m_TLASBank.reset();
//        m_MeshBank.reset();
//        m_ObjectBank.reset();
//        m_MaterialBank.reset();
//
//        glDeleteVertexArrays(1, &m_VAO);
//        glDeleteBuffers(1, &m_VBO);
//        glDeleteBuffers(1, &m_EBO);
//
//        m_RayTracingShader.reset();
//
//        glfwDestroyWindow(window);
//    }
//
//    void GPURayTracing::Render() {
//        ++m_FrameCount;
//
//        // Render into accumulation framebuffer
//        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
//        glViewport(0, 0, App::screenWidth, App::screenHeight);
//
//        m_RayTracingShader->Bind();
//
//        const glm::mat4 cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
//        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);
//
//        const glm::mat4 inverseView = glm::inverse(App::camera.View());
//
//        m_RayTracingShader->SetFloat("miliTime", (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_RendererLoadTime).count() / 1000.0f);
//        m_RayTracingShader->SetInt("screenWidth", App::screenWidth);
//        m_RayTracingShader->SetInt("screenHeight", App::screenHeight);
//
//        m_RayTracingShader->SetMat4("invView", inverseView);
//        m_RayTracingShader->SetMat4("invProjection", inverseProjection);
//        m_RayTracingShader->SetVec3("cameraPosition", App::camera.position);
//
//        m_RayTracingShader->SetVec3("backgroundColor", App::settings.backgroundColor);
//
//        m_RayTracingShader->SetInt("objectCount", (int)App::scene.objects.size());
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
//        m_RayTracingShader->SetInt("accumulationBuffer", 0);
//
//        glBindVertexArray(m_VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//
//        // Read from accumulation framebuffer, divide by frame count, and render to default framebuffer
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, App::screenWidth, App::screenHeight);
//
//        glClearColor(App::settings.backgroundColor.b, App::settings.backgroundColor.g, App::settings.backgroundColor.b, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        m_RenderingShader->Bind();
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
//        m_RenderingShader->SetInt("accumulationBuffer", 0);
//
//        m_RenderingShader->SetInt("frameCount", m_FrameCount);
//
//        glBindVertexArray(m_VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//    }
//
//    void GPURayTracing::LoadScene() {
//        ResetAccumulatedPixelData();
//
//        CreateAndUploadMaterialBuffer();
//
//        CreateAndUploadBVHAndMeshAndObjectBuffers();
//    }
//
//    void GPURayTracing::SignalRayTracingSettingsChange() {
//        m_RayTracingShader->Bind();
//        m_RayTracingShader->SetInt("maxBounces", App::settings.maxBounces);
//
//        ResetAccumulatedPixelData();
//    }
//
//    void GPURayTracing::ProvideLocalRendererSettings() {
//        static int maxBboxChecks = 100;
//        static int maxSphereChecks = 100;
//        static int maxTriangleChecks = 100;
//        static int maxMeshChecks = 100;
//
//        static int mode = 0;
//
//        RadioButtons("Stats mode", std::vector<std::string>{
//                "Bounding Boxes",
//                "Spheres",
//                "Triangles",
//                "Meshes"
//            },
//            &mode
//        );
//
//        int bbox = -1;
//        int sphere = -1;
//        int tri = -1;
//        int mesh = -1;
//
//        switch (mode) {
//        case 0:
//            if (ImGui::DragInt("Max Bounding Box Checks", &maxBboxChecks, 0.1f, 0, 10000)) {
//                ResetAccumulatedPixelData();
//            }
//            bbox = maxBboxChecks;
//            break;
//
//        case 1:
//            if (ImGui::DragInt("Max Sphere Checks", &maxSphereChecks, 0.1f, 0, 10000)) {
//                ResetAccumulatedPixelData();
//            }
//            sphere = maxSphereChecks;
//            break;
//
//        case 2:
//            if (ImGui::DragInt("Max Triangle Checks", &maxTriangleChecks, 0.1f, 0, 10000)) {
//                ResetAccumulatedPixelData();
//            }
//            tri = maxTriangleChecks;
//            break;
//
//        case 3:
//            if (ImGui::DragInt("Max Mesh Checks", &maxMeshChecks, 0.1f, 0, 10000)) {
//                ResetAccumulatedPixelData();
//            }
//            mesh = maxMeshChecks;
//            break;
//
//        default:
//            std::cout << "ERROR: Unknown mode" << std::endl;
//            break;
//        }
//
//        m_RayTracingShader->Bind();
//        m_RayTracingShader->SetInt("maxBboxChecks", bbox);
//        m_RayTracingShader->SetInt("maxSphereChecks", sphere);
//        m_RayTracingShader->SetInt("maxTriangleChecks", tri);
//        m_RayTracingShader->SetInt("maxMeshChecks", mesh);
//    }
//
//    void GPURayTracing::ResetAccumulatedPixelData() {
//        m_FrameCount = 0;
//
//        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
//        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
//
//        std::vector<glm::vec4> clearData{ };
//        clearData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);
//        for (auto& val : clearData) {
//            val = glm::vec4{ 0.0f };
//        }
//
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, clearData.data());
//    }
//
//    void GPURayTracing::CreateAndUploadMaterialBuffer() {
//        m_RayTracingShader->Bind();
//
//        std::vector<LocalMaterial> localMats{ };
//        for (size_t i = 0; i < App::scene.materialBank.Size(); ++i) {
//            LocalMaterial mat{ };
//            mat.type = (int)App::scene.materialBank[i].type;
//            mat.fuzz = App::scene.materialBank[i].fuzz;
//            mat.indexOfRefraction = App::scene.materialBank[i].indexOfRefraction;
//            mat.color = glm::vec4{ App::scene.materialBank[i].solid.color, 1.0 };
//
//            localMats.emplace_back(mat);
//        }
//
//        m_MaterialBank->SetData(localMats);
//    }
//
//    void GPURayTracing::CreateAndUploadBVHAndMeshAndObjectBuffers() {
//        m_RayTracingShader->Bind();
//
//        std::vector<Object> objects = App::scene.objects;
//        auto nodes = TemplateBVHFactory<Object>::Construct(objects, 1);
//
//        std::vector<LocalTLASNode> TLASNodes;
//
//        for (auto node : nodes) {
//            LocalTLASNode n{ };
//
//            n.min = node.bbox.min;
//            n.max = node.bbox.max;
//
//            n.node2 = node.count;
//
//            if (node.count > 0) {
//                n.node1 = node.offset;
//            }
//            else {
//                n.node1 = node.node1;
//            }
//
//            TLASNodes.push_back(n);
//        }
//
//        m_TLASBank->SetData(TLASNodes);
//
//        m_RayTracingShader->SetInt("BVHStartIndex", (int)0);
//
//        std::vector<float> triangleData;
//        std::vector<LocalBLASNode> blasNodes;
//        std::vector<int> startingIndices;
//
//        for (size_t i = 0; i < App::scene.geometryBank.Size(); ++i) {
//            Geometry& geo = App::scene.geometryBank[i];
//
//            if (geo.type == Geometry::GeometryType::SPHERE) {
//                startingIndices.push_back(-1);
//                continue;
//            }
//
//            std::vector<Triangle> tris;
//            for (size_t i = 0; i < geo.indices.size(); i += 3) {
//                const Vertex v1 = geo.vertices[geo.indices[i + 0]];
//                const Vertex v2 = geo.vertices[geo.indices[i + 1]];
//                const Vertex v3 = geo.vertices[geo.indices[i + 2]];
//
//                tris.push_back(Triangle{ v1.position, v2.position, v3.position });
//            }
//
//            auto nodes = TemplateBVHFactory<Triangle>::Construct(tris);
//
//            int startingIndex = (int)blasNodes.size();
//
//            startingIndices.push_back(startingIndex);
//
//            std::vector<float> meshData{ };
//
//            for (auto tri : tris) {
//                glm::vec3 v1 = tri[0];
//                glm::vec3 v2 = tri[1];
//                glm::vec3 v3 = tri[2];
//
//                meshData.push_back(v1.x);
//                meshData.push_back(v1.y);
//                meshData.push_back(v1.z);
//
//                meshData.push_back(v2.x);
//                meshData.push_back(v2.y);
//                meshData.push_back(v2.z);
//
//                meshData.push_back(v3.x);
//                meshData.push_back(v3.y);
//                meshData.push_back(v3.z);
//            }
//
//            std::vector<LocalBLASNode> localObjBvhNodes{ };
//
//            for (auto node : nodes) {
//                int node1 = -1;
//                int node2 = -1;
//
//                if (node.node1 != -1) {
//                    node1 = (int)node.node1 + (int)blasNodes.size();
//                }
//
//                int triangleOffset = -1;
//                int triangleCount = 0;
//
//                if (node.offset != -1) {
//                    triangleOffset = (node.offset * 9) + (int)triangleData.size(); // 9 floats in a triangle
//                }
//
//                if (node.count != 0) {
//                    // 9 floats in a triangle
//                    triangleCount = node.count * 9;
//                }
//
//                LocalBLASNode n{ };
//
//                n.min = node.bbox.min;
//                n.max = node.bbox.max;
//
//                n.triangleCount = triangleCount;
//
//                if (n.triangleCount > 0) {
//                    n.node1Offset = triangleOffset;
//                }
//                else {
//                    n.node1Offset = node1;
//                }
//
//                localObjBvhNodes.push_back(n);
//            }
//
//            triangleData.insert(triangleData.end(), meshData.begin(), meshData.end());
//
//            blasNodes.insert(blasNodes.end(), localObjBvhNodes.begin(), localObjBvhNodes.end());
//        }
//
//        m_MeshBank->SetData(triangleData);
//
//        m_BLASBank->SetData(blasNodes);
//
//        std::vector<LocalObject> localObjects{ };
//        int i = 0;
//        for (auto object : objects) {
//            int geoType;
//            if (App::scene.geometryBank[object.geometry].type == Geometry::GeometryType::SPHERE) {
//                geoType = 0;
//            }
//            else {
//                geoType = 1;
//            }
//
//            localObjects.push_back(LocalObject{
//                App::scene.transformBank[object.transform].matrix,
//                glm::inverse(App::scene.transformBank[object.transform].matrix),
//                glm::mat4{ glm::transpose(glm::inverse(glm::mat3{ App::scene.transformBank[object.transform].matrix })) },
//                glm::mat4{ glm::transpose(glm::mat3{ App::scene.transformBank[object.transform].matrix }) },
//                (int)object.material,
//                geoType,
//                startingIndices[object.geometry]
//            });
//
//            ++i;
//        }
//
//        m_ObjectBank->SetData(localObjects);
//    }
//}