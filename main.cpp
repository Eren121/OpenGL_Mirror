#include "Context.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include <utility/gl/Shader.hpp>
#include <utility/time/Clock.hpp>
#include "Model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#   include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

const int shadowTexIndex = 2;

struct Mirror
{
    glm::vec3 pos{0};
    float scale{10.0f};
    glm::vec3 rotation{0}; // In degrees
    
    glm::mat4 model() const
    {
        glm::mat4 mat{1};
        mat = glm::translate(mat, pos);
        mat = glm::scale(mat, glm::vec3{scale});
        mat *= glm::mat4{glm::quat{glm::radians(rotation)}};
        
        return mat;
    }
    
    glm::vec3 origin() const
    {
        return model() * glm::vec4{glm::vec3{0}, 1};
    }
    
    glm::vec3 n1() const
    {
        return normalize(model() * glm::vec4{glm::vec3{1, 0, 0}, 0});
    }
    
    glm::vec3 n2() const
    {
        return normalize(model() * glm::vec4{glm::vec3{0, 1, 0}, 0});
    }
    
    // transform any point into its reflection, in the mirror coordinate system
    // more output z is negative more the reflection is far into the mirror
    glm::mat4 getReflectionMatrixInMirrorCoords() const
    {
    
        const auto n1 = this->n1();
        const auto n2 = this->n2();
        const auto mirrorPos = this->origin();
    
        const glm::vec3 n3{normalize(glm::cross(n1, n2))};
    
        // n1, n2 are orthogonal vectors of the mirror
        // n3 is to complete the basis
        // the vertices should be in trigonometric order to work
        glm::mat4 reflectionMatrix{1};
    
        // Let p be the point we mirror
        // Compute p1 = p relative to the mirror [Translation in global coordinate system ~= premultiply]
        reflectionMatrix = glm::translate(glm::mat4{1}, -mirrorPos) * reflectionMatrix;
    
        // Compute p2 = same as p1 but in coordinate system {n1, n2, n3}
        reflectionMatrix = glm::inverse(glm::transition(n1, n2, n3)) * reflectionMatrix;
    
        // Compute p3 = (p3.x, p3.y, -p3.z) to go into the mirror (z is orthogonal distance from mirror)
        reflectionMatrix = glm::scale(glm::mat4{1}, {1, 1, -1}) * reflectionMatrix;
    
        return reflectionMatrix;
    }
    
    glm::mat4 getReflectionMatrix() const
    {
        const auto n1 = this->n1();
        const auto n2 = this->n2();
        const auto mirrorPos = this->origin();
        
        const glm::vec3 n3{normalize(glm::cross(n1, n2))};
        
        // n1, n2 are orthogonal vectors of the mirror
        // n3 is to complete the basis
        // the vertices should be in trigonometric order to work
        glm::mat4 reflectionMatrix{1};
        
        // Let p be the point we mirror
        // Compute p1 = p relative to the mirror [Translation in global coordinate system ~= premultiply]
        reflectionMatrix = glm::translate(glm::mat4{1}, -mirrorPos) * reflectionMatrix;
        
        // Compute p2 = same as p1 but in coordinate system {n1, n2, n3}
        reflectionMatrix = glm::inverse(glm::transition(n1, n2, n3)) * reflectionMatrix;
        
        // Compute p3 = (p3.x, p3.y, -p3.z) to go into the mirror (z is orthogonal distance from mirror)
        reflectionMatrix = glm::scale(glm::mat4{1}, {1, 1, -1}) * reflectionMatrix;
        
        // Compute p4 = same as p3 but in canonical coordinate system (again relative to the mirror)
        reflectionMatrix = glm::transition(n1, n2, n3) * reflectionMatrix;
        
        // Compute p5 = p4 from relative to the mirror to global space [Translation in global coordinate system]
        reflectionMatrix = glm::translate(glm::mat4{1}, mirrorPos) * reflectionMatrix;
        
        return reflectionMatrix;
    }
  
    void clearDepth(gl::Shader& shader) const
    {
        // Clear the depth buffer where the stencil buffer is 1
        glStencilFunc(GL_EQUAL, 1, 0xff); // To set the reference to 1
    
        Uniforms uni;
        uni.send(shader);
    
        // We can't glDisable(GL_DEPTH_TEST) because it will also disable writing to the depth buffer
        // We don't change about the color we just want to clear the depth by writing to max depth that is 1
        glDepthFunc(GL_ALWAYS);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        
        obj::Vertex vertices[4];
        vertices[0].pos = {-1, -1, 1};
        vertices[1].pos = {1, -1, 1};
        vertices[2].pos = {1, 1, 1};
        vertices[3].pos = {-1, 1, 1};
    
        gl::raii::VertexArray vao;
        gl::raii::Buffer vbo;
    
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
        glEnableVertexAttribArray(obj::AttrVertex);
        gl::vertexAttribPointer(obj::AttrVertex, 3, GL_FLOAT, &obj::Vertex::pos);
    
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);
    
        glDepthFunc(GL_LESS);
        glStencilFunc(GL_ALWAYS, 0, 0xff); // To set the reference to 1
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    
    void drawReflection(Scene& scene, gl::Shader& shader, Uniforms uniforms) const
    {
        // Draw only where the mirror was drawn == where stencil buffer equals 1
        glStencilFunc(GL_EQUAL, 1, 0xff); // To set the reference to 1
        
        // Mix with the mirror base color
        uniforms.opacity *= 0.9;
        
        shader.setUniform("u_ReflectionMatrix", getReflectionMatrix());
        shader.setUniform("u_ReflectionMatrixLocal", getReflectionMatrixInMirrorCoords());
        scene.draw(shader, uniforms);
    
        glStencilFunc(GL_ALWAYS, 0, 0xff); // Reset
    }
    
} mirror;

struct GUI
{
    bool showReflection{false};
    bool showAxis{true};
    bool showDemoWindow{false};
} gui;

struct Camera
{
    Scene *scene{nullptr};
    glm::quat quat{1, 0, 0, 0};
    float distance{4.0f};

    glm::vec3 up() const
    {
        const glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
        const glm::vec3 up{quat * worldUp};
        return up;
    }
 
    glm::vec3 eye() const
    {
        glm::vec3 eye{0};
        eye.z = distance;
        eye = quat * eye;
        return eye;
    }
    
    glm::mat4 view() const
    {
        return glm::lookAt(eye(), {0.0f, 0.0f, 0.0f}, up());
    }
    
    glm::mat4 proj(glm::ivec2 viewport) const
    {
        return glm::perspective(
                glm::radians(80.0f),
                static_cast<float>(viewport.x) / static_cast<float>(viewport.y),
                0.1f,
                100.0f);
    }
    
} camera;

Context *ctxt;

Clock animClock;

Uniforms getUniforms()
{
    Uniforms ret;
    
    ret.lightDir = normalize(glm::vec3{-0.1, -1, -0.1});
    
    ret.proj = camera.proj(ctxt->winSize);
    ret.view = camera.view();
    
    return ret;
}

void pollEvents(GLFWwindow *win)
{
    const float zoomStep = 0.1f;
    const float delta = 0.01f;
    const glm::quat horizontal{{0, delta, 0}};
    const glm::quat vertical{{delta, 0, 0}};
    
    if(glfwGetKey(win, GLFW_KEY_LEFT))
    {
        camera.quat = glm::inverse(horizontal) * camera.quat;
    }
    if(glfwGetKey(win, GLFW_KEY_RIGHT))
    {
        // Turn locally for vertical, turn globally for horizontal
        camera.quat = horizontal * camera.quat;
    }
    
    if(glfwGetKey(win, GLFW_KEY_UP))
    {
        camera.quat *= glm::inverse(vertical);
    }
    if(glfwGetKey(win, GLFW_KEY_DOWN))
    {
        camera.quat *= vertical;
    }
    
    if(glfwGetKey(win, GLFW_KEY_EQUAL))
    {
        camera.distance -= zoomStep;
    }
    
    if(glfwGetKey(win, GLFW_KEY_MINUS))
    {
        camera.distance += zoomStep;
    }
}

void post_call_callback_debug(const char *name, void *funcptr, int len_args, ...)
{
    GLenum error_code;
    error_code = glad_glGetError();
    
    if (error_code != GL_NO_ERROR)
    {
        fprintf(stderr, "ERROR %d in %s\n", error_code, name);
    }
}

void drawGUI()
{
    ImGuiStyle style;
    ImGui::StyleColorsClassic(&style);
    style.FrameBorderSize = 1.0f;
    
    ImGui::GetStyle() = style;
    
    if(ImGui::CollapsingHeader("Mirror", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat3("Position", &mirror.pos.x, -10.0f, 10.0f, "%.0f");
        ImGui::SliderFloat("Scale", &mirror.scale, 1.f, 10.0f, "%.0f");
        ImGui::SliderFloat3("Rotation", &mirror.rotation.x, -180.0f, 180.0f, "%.0f");
        
        glm::vec3 n1{mirror.n1()}, n2{mirror.n2()};
        ImGui::InputFloat3("n1", &n1.x, "%.6f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat3("n2", &n2.x, "%.6f", ImGuiInputTextFlags_ReadOnly);
    
        ImGui::Text("Reflection Matrix");
        if(ImGui::BeginTable("Reflection Matrix", 4))
        {
            const auto refMat = mirror.getReflectionMatrix();
            for(int row = 0; row < 4; ++row)
            {
                ImGui::TableNextRow();
                for(int col = 0; col < 4; ++col)
                {
                    ImGui::TableSetColumnIndex(col);
                    ImGui::Text("%f", refMat[col][row]);
                }
            }
            ImGui::EndTable();
        }
        
        if(ImGui::CollapsingHeader("Test reflection"))
        {
            static glm::vec4 point{0, 0, 0, 1};
            ImGui::InputFloat4("Point (world coords)", &point.x);
            
            glm::vec4 reflection{mirror.getReflectionMatrix() * point};
            ImGui::InputFloat4("Reflected point (world coords)", &reflection.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
        }
        
        if(ImGui::Button("Reset"))
        {
            mirror = Mirror{};
        }
        ImGui::Checkbox("Show reflection only", &gui.showReflection);
        ImGui::Checkbox("Show axis", &gui.showAxis);
    }
    
    if(ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(camera.quat));
        ImGui::InputFloat3("Euler camera", &eulerAngles.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
    }
    
    if(ImGui::CollapsingHeader("Debug"))
    {
        ImGui::Checkbox("Show demo window", &gui.showDemoWindow);
        if(gui.showDemoWindow) ImGui::ShowDemoWindow(&gui.showDemoWindow);
        
        if (ImGui::CollapsingHeader("Textures"))
        {
            for (int i = 0; i < 64; ++i)
            {
                if (glIsTexture(i))
                {
                    int w, h;
                    glBindTexture(GL_TEXTURE_2D, i);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &h);
                
                    ImGui::Text("Texture %d: %dx%d", i, w, h);
                    ImGui::Image((ImTextureID) i, {100, 100});
                }
            }
        }
    }
}

int main()
{
    glad_set_post_callback(post_call_callback_debug);
    
    Context ctxt;
    ::ctxt = &ctxt;
    
    gl::enableDebugging();
    
    glfwSetErrorCallback([](int code, const char *description) {
        std::cerr << description << std::endl;
    });
    
    glfwSetKeyCallback(ctxt.window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
        if(action == GLFW_PRESS)
        {
            if(key == GLFW_KEY_P)
            {
                animClock.toggle();
            }
        }
    });
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(ctxt.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    
    const std::filesystem::path assets{std::filesystem::current_path() / "../assets"};
    
    Scene scene{assets};
    camera.scene = &scene;
    
    gl::Shader shader;
    shader.load(assets / "base.vert", assets / "base.frag");
    
    gl::Shader reflectionShader;
    reflectionShader.load(assets / "reflection.vert", assets / "reflection.frag");
    
    gl::raii::Framebuffer mirrorFbo;
    gl::Texture texMirrorFbo;
    texMirrorFbo.load(ctxt.winSize, GL_RGBA);
    texMirrorFbo.setFilter(gl::Texture::Linear);
    
    gl::Texture depthMirrorFbo;
    depthMirrorFbo.load(ctxt.winSize, GL_DEPTH_COMPONENT);
    depthMirrorFbo.setFilter(gl::Texture::Linear);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mirrorFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texMirrorFbo.getID(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMirrorFbo.getID(), 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    gl::Texture dummy;
    dummy.load1x1White();
    
    // GL_TEXTURE1 for ambient color (general texture)
    
    while(!glfwWindowShouldClose(ctxt.window))
    {
        glfwPollEvents();
        pollEvents(ctxt.window);
    
        int display_w, display_h;
        glfwGetFramebufferSize(ctxt.window, &display_w, &display_h);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        glViewport(0, 0, display_w, display_h);
        scene.resetGL();
        scene.clear();
        
        // Use texture 0 as the "non-texture"
        // Since we mostly multiply the texture, we use opaque white 1x1 as default
        // So we can use any shader using textures without needing specific ones, and we can also use effects
        glActiveTexture(GL_TEXTURE0);
        gl::Texture::bind(&dummy);
        
        Uniforms uniforms = getUniforms();
        
        if(gui.showReflection)
        {
            uniforms.model = mirror.getReflectionMatrix();
        }
        
        scene.draw(shader, uniforms);
        
        uniforms = getUniforms();
        uniforms.model = mirror.model();
        uniforms.ambient = 1.;
        uniforms.diffuseColor = glm::vec4{1, 0, 1, 1};
        
        scene.drawMirror(shader, uniforms);
        
        uniforms = getUniforms();
        mirror.clearDepth(shader);
        mirror.drawReflection(scene, reflectionShader, uniforms);
    
        drawGUI();
        
        // Draw axis on top of everything with glClearDepth()
        // glDisableDepth() would work partially because the object itself will be not depth-tested so awkward rendering
        if(gui.showAxis)
        {
            struct
            {
                glm::ivec2 pos;
                glm::ivec2 size;
            } viewport;
            
            const glm::ivec2 margin{50};
            viewport.size = glm::ivec2{100};
            viewport.pos = ctxt.winSize - viewport.size - margin;
    
            glViewport(viewport.pos.x, viewport.pos.y, viewport.size.x, viewport.size.y);
            
            glEnable(GL_SCISSOR_TEST);
            glScissor(viewport.pos.x, viewport.pos.y, viewport.size.x, viewport.size.y);
    
            glClearColor(1, 1, 1, 1);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glDisable(GL_SCISSOR_TEST);
    
    
            uniforms = {};
            
            static obj::Model axis{assets / "axis.obj"};
            const float d = 1;
            
            uniforms.proj = glm::ortho(-d, d, -d, d, -d, d);
            uniforms.model *= glm::mat4{glm::inverse(camera.quat)};
            uniforms.ambient = 1;
            uniforms.diffuse = 0;
            uniforms.lightDir = glm::vec3{1, -1, 0};
            uniforms.send(shader);
            axis.draw(shader);
        }
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(ctxt.window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}
