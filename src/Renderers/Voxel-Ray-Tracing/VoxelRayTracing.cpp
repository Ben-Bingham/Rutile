#include "VoxelRayTracing.h"
#include "imgui.h"
#include <bitset>
#include <iostream>

#include "GUI/ImGuiUtil.h"

#include "Settings/App.h"

#include "Utility/events/Events.h"
#include "Utility/OpenGl/GLDebug.h"

namespace Rutile {
    uint32_t SetBit(uint32_t value, size_t n) {
        return value |= (1 << n);
    }

    GLFWwindow* VoxelRayTracing::Init() {
        uint32_t i = 293;
        std::cout << "I: " << std::bitset<32>(i) << std::endl;

        //uint32_t l = SetBit(i, 0);
        //std::cout << "L: " << std::bitset<32>(l) << std::endl;

        //uint32_t r = SetBit(i, 31);;
        //std::cout << "R: " << std::bitset<32>(r) << std::endl;



        // The blocks of the chunk
        //i = SetBit(i, 24);
        //i = SetBit(i, 26);
        //i = SetBit(i, 29);
        //i = SetBit(i, 31);

        //std::cout << "I: " << std::bitset<32>(i) << std::endl;

        //std::cout << std::bitset<32>(i >> 24) << std::endl;

        //uint32_t index = i << 8;
        //index = index >> 8;
        //std::cout << "Index: " << std::bitset<32>(index) << std::endl;

        //std::cout << index << std::endl;









        m_RendererLoadTime = std::chrono::steady_clock::now();

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

        m_BlockData = std::make_unique<SSBO<unsigned char>>(0);

        std::vector<float> vertices = {
            // Positions
            -1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        m_VoxelRayTracingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\VoxelRayTracing\\VoxelRayTracing.vert", "assets\\shaders\\renderers\\VoxelRayTracing\\VoxelRayTracing.frag");
        m_RenderingShader = std::make_unique<Shader>("assets\\shaders\\renderers\\VoxelRayTracing\\Rendering.vert", "assets\\shaders\\renderers\\VoxelRayTracing\\Rendering.frag");

        // Screen Rectangle
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Accumulation Framebuffer
        glGenFramebuffers(1, &m_AccumulationFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);

        glGenTextures(1, &m_AccumulationTexture);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenRenderbuffers(1, &m_AccumulationRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR: Accumulation Framebuffer is not complete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_VoxelRayTracingShader->Bind();

        m_VoxelRayTracingShader->SetInt("octTreeX", m_OctTreeX);
        m_VoxelRayTracingShader->SetInt("octTreeY", m_OctTreeY);
        m_VoxelRayTracingShader->SetInt("octTreeZ", m_OctTreeZ);

        ResetAccumulatedPixelData();

        return window;
    }

    void VoxelRayTracing::Cleanup(GLFWwindow* window) {
        m_VoxelRayTracingShader.reset();
        m_RenderingShader.reset();

        m_BlockData.reset();

        glfwDestroyWindow(window);
    }

    void VoxelRayTracing::Notify(Event* event) {
        if (EVENT_IS(event, WindowResize)) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
            glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationTexture, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, m_AccumulationRBO);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App::screenWidth, App::screenHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_AccumulationRBO);

            ResetAccumulatedPixelData();
        }
        if (EVENT_IS(event, CameraUpdate) ||
            EVENT_IS(event, ObjectTransformUpdate) ||
            EVENT_IS(event, ObjectMaterialUpdate)) {

            ResetAccumulatedPixelData();
        }
    }
    void VoxelRayTracing::Render() {
        m_VoxelRayTracingShader->Bind();

        m_VoxelRayTracingShader->SetInt("child1", m_Child1);
        m_VoxelRayTracingShader->SetInt("child2", m_Child2);
        m_VoxelRayTracingShader->SetInt("child3", m_Child3);
        m_VoxelRayTracingShader->SetInt("child4", m_Child4);
        m_VoxelRayTracingShader->SetInt("child5", m_Child5);
        m_VoxelRayTracingShader->SetInt("child6", m_Child6);
        m_VoxelRayTracingShader->SetInt("child7", m_Child7);
        m_VoxelRayTracingShader->SetInt("child8", m_Child8);

        m_VoxelRayTracingShader->SetFloat("startingWidth", m_StartingWidth);

        ++m_FrameCount;

        // Render into accumulation framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        m_VoxelRayTracingShader->Bind();

        const glm::mat4 cameraProjection = glm::perspective(glm::radians(App::settings.fieldOfView), (float)App::screenWidth / (float)App::screenHeight, App::settings.nearPlane, App::settings.farPlane);
        const glm::mat4 inverseProjection = glm::inverse(cameraProjection);

        const glm::mat4 inverseView = glm::inverse(App::camera.View());

        m_VoxelRayTracingShader->SetFloat("miliTime", (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_RendererLoadTime).count() / 1000.0f);
        m_VoxelRayTracingShader->SetInt("screenWidth", App::screenWidth);
        m_VoxelRayTracingShader->SetInt("screenHeight", App::screenHeight);

        m_VoxelRayTracingShader->SetMat4("invView", inverseView);
        m_VoxelRayTracingShader->SetMat4("invProjection", inverseProjection);
        m_VoxelRayTracingShader->SetVec3("cameraPosition", App::camera.position);

        m_VoxelRayTracingShader->SetInt("maxBounces", App::settings.maxBounces);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
        m_VoxelRayTracingShader->SetInt("accumulationBuffer", 0);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Read from accumulation framebuffer, divide by frame count, and render to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, App::screenWidth, App::screenHeight);

        glClearColor(App::settings.backgroundColor.b, App::settings.backgroundColor.g, App::settings.backgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_RenderingShader->Bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);
        m_RenderingShader->SetInt("accumulationBuffer", 0);

        m_RenderingShader->SetInt("frameCount", (int)m_FrameCount);

        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /*
         * Method for finding out what block we hit:
         *
         * Materials:
         *      An array of `oct trees`
         *      `Oct tree`: 1 int. first 8 bits dictate which children the octree has. bit 1 will always corospond to the same octant
         *      the next 24 bits will be the index into the oct tree array of the first child of the octree, the next children will follow that index.
         *          Rules for the first 8 bits.
         *              Using right hand rule, look down the negative Y axis. Starting with the smallest Y, the order goes in clockwise order:
         *                    Closer to y = 0 (lower)       Closer to y = inf (higher)
         *                    ---------                     ---------
         *                    | 0 | 1 |                     | 4 | 5 |
         *                    ---------> +X                 ---------> +X
         *                    | 2 | 3 |                     | 6 | 7 |
         *                    ---------                     ---------
         *                        V                             V
         *                        +Z                            +Z
         *
         *      There will than be an array of materials, probably quite small ( < 100), but who knows, the order can be arbirtrary, but constant.
         *      And there will be an array, that is filled with indices into the material array.
         *
         *      After N levels, the index will become the index of the first childs material in the array that has indices into the material array.
         *      Then the indices of the next materials will be in order in the array of indices into the material array.
         *
         *      For the octrees, if the first 8 bits are all 0, AND the index is 0, than that octree is empty.
         *      BUT if the index is non 0 (and the first 8 bits are all 0), than that index is a material index, and the area is solid
         *          The index being 0 will never be valid, because 0 is the index of thr Root octree, and octress dont circle back
         *
         *  To find the total number of blocks that a nested octree can hold, do 8^n, where n is the number of levels.
         *      8^8 = 16777216 total blocks = 256 blocks per side
         *     
         */
    }

    void VoxelRayTracing::LoadScene() {
        ResetAccumulatedPixelData();
    }

    void VoxelRayTracing::ProvideLocalRendererSettings() {
        if (ImGui::DragInt("Child #1", &m_Child1, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #2", &m_Child2, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #3", &m_Child3, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #4", &m_Child4, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #5", &m_Child5, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #6", &m_Child6, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #7", &m_Child7, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }
        if (ImGui::DragInt("Child #8", &m_Child8, 0.1f, 0, 7)) { ResetAccumulatedPixelData(); }

        if (ImGui::DragFloat("Starting width", &m_StartingWidth, 0.1f, 0.0001f, 100000000.0f)) { ResetAccumulatedPixelData(); }

        if (ImGui::Checkbox("No Kids", &m_OctTreeNoKids)) { ResetAccumulatedPixelData(); }

        ImGui::Text("Level 1");
        if (ImGui::Checkbox("X##L1", &m_OctTreeX)) { ResetAccumulatedPixelData(); }
        if (ImGui::Checkbox("Y##L1", &m_OctTreeY)) { ResetAccumulatedPixelData(); }
        if (ImGui::Checkbox("Z##L1", &m_OctTreeZ)) { ResetAccumulatedPixelData(); }

        ImGui::Separator();
        ImGui::Text("Level 2");
        if (ImGui::Checkbox("X##L2", &m_OctTreeXL2)) { ResetAccumulatedPixelData(); }
        if (ImGui::Checkbox("Y##L2", &m_OctTreeYL2)) { ResetAccumulatedPixelData(); }
        if (ImGui::Checkbox("Z##L2", &m_OctTreeZL2)) { ResetAccumulatedPixelData(); }

        m_VoxelRayTracingShader->Bind();

        m_VoxelRayTracingShader->SetInt("child1", m_Child1);
        m_VoxelRayTracingShader->SetInt("child2", m_Child2);
        m_VoxelRayTracingShader->SetInt("child3", m_Child3);
        m_VoxelRayTracingShader->SetInt("child4", m_Child4);
        m_VoxelRayTracingShader->SetInt("child5", m_Child5);
        m_VoxelRayTracingShader->SetInt("child6", m_Child6);
        m_VoxelRayTracingShader->SetInt("child7", m_Child7);
        m_VoxelRayTracingShader->SetInt("child8", m_Child8);

        m_VoxelRayTracingShader->SetFloat("startingWidth", m_StartingWidth);

        m_VoxelRayTracingShader->SetInt("octTreeX", m_OctTreeX);
        m_VoxelRayTracingShader->SetInt("octTreeY", m_OctTreeY);
        m_VoxelRayTracingShader->SetInt("octTreeZ", m_OctTreeZ);

        m_VoxelRayTracingShader->SetInt("octTreeXL2", m_OctTreeXL2);
        m_VoxelRayTracingShader->SetInt("octTreeYL2", m_OctTreeYL2);
        m_VoxelRayTracingShader->SetInt("octTreeZL2", m_OctTreeZL2);

        m_VoxelRayTracingShader->SetInt("octTreeNoKids", m_OctTreeNoKids);

        const int OCT_CHILD_0 = 24;
        const int OCT_CHILD_1 = 25;
        const int OCT_CHILD_2 = 26;
        const int OCT_CHILD_3 = 27;

        const int OCT_CHILD_4 = 28;
        const int OCT_CHILD_5 = 29;
        const int OCT_CHILD_6 = 30;
        const int OCT_CHILD_7 = 31;

        int octree = 0;

        static bool r0 = false;
        static bool r1 = false;
        static bool r2 = false;
        static bool r3 = false;
        static bool r4 = false;
        static bool r5 = false;
        static bool r6 = false;
        static bool r7 = false;

        ImGui::Separator();

        if (ImGui::Checkbox("0", &r0)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("1", &r1)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("2", &r2)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("3", &r3)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("4", &r4)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("5", &r5)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("6", &r6)) { ResetAccumulatedPixelData(); } ImGui::SameLine();
        if (ImGui::Checkbox("7", &r7)) { ResetAccumulatedPixelData(); }

        if (r0) { octree = SetBit(octree, OCT_CHILD_0); }
        if (r1) { octree = SetBit(octree, OCT_CHILD_1); }
        if (r2) { octree = SetBit(octree, OCT_CHILD_2); }
        if (r3) { octree = SetBit(octree, OCT_CHILD_3); }
        if (r4) { octree = SetBit(octree, OCT_CHILD_4); }
        if (r5) { octree = SetBit(octree, OCT_CHILD_5); }
        if (r6) { octree = SetBit(octree, OCT_CHILD_6); }
        if (r7) { octree = SetBit(octree, OCT_CHILD_7); }

        m_VoxelRayTracingShader->SetInt("octree", octree);

        int octreeChild0 = 0;

        //m_VoxelRayTracingShader->SetInt("octreeChild", octreeChild);





        static int maxBboxChecks = 100;
        static int maxSphereChecks = 100;
        static int maxTriangleChecks = 100;
        static int maxMeshChecks = 100;

        static int mode = 0;

        RadioButtons("Stats mode", std::vector<std::string>{
            "Bounding Boxes",
            "Spheres",
            "Triangles",
            "Meshes"
        },
            & mode
        );

        int bbox = -1;
        int sphere = -1;
        int tri = -1;
        int mesh = -1;

        switch (mode) {
        case 0:
            if (ImGui::DragInt("Max Bounding Box Checks", &maxBboxChecks, 0.1f, 0, 10000)) {
                ResetAccumulatedPixelData();
            }
            bbox = maxBboxChecks;
            break;

        case 1:
            if (ImGui::DragInt("Max Sphere Checks", &maxSphereChecks, 0.1f, 0, 10000)) {
                ResetAccumulatedPixelData();
            }
            sphere = maxSphereChecks;
            break;

        case 2:
            if (ImGui::DragInt("Max Triangle Checks", &maxTriangleChecks, 0.1f, 0, 10000)) {
                ResetAccumulatedPixelData();
            }
            tri = maxTriangleChecks;
            break;

        case 3:
            if (ImGui::DragInt("Max Mesh Checks", &maxMeshChecks, 0.1f, 0, 10000)) {
                ResetAccumulatedPixelData();
            }
            mesh = maxMeshChecks;
            break;

        default:
            std::cout << "ERROR: Unknown mode" << std::endl;
            break;
        }

        m_VoxelRayTracingShader->Bind();
        m_VoxelRayTracingShader->SetInt("maxBboxChecks", bbox);
        m_VoxelRayTracingShader->SetInt("maxSphereChecks", sphere);
        m_VoxelRayTracingShader->SetInt("maxTriangleChecks", tri);
        m_VoxelRayTracingShader->SetInt("maxMeshChecks", mesh);
    }

    void VoxelRayTracing::ResetAccumulatedPixelData() {
        m_FrameCount = 0;

        glBindFramebuffer(GL_FRAMEBUFFER, m_AccumulationFrameBuffer);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationTexture);

        std::vector<glm::vec4> clearData{ };
        clearData.resize((size_t)App::screenWidth * (size_t)App::screenHeight);
        for (auto& val : clearData) {
            val = glm::vec4{ 0.0f };
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, App::screenWidth, App::screenHeight, 0, GL_RGBA, GL_FLOAT, clearData.data());
    }
}