#include "Context.hpp"
#include <iostream>

Context::Context()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Window width and height, float first to not have rounding errors
    glm::vec2 size{0};
    size.x = 1920;
    size.y = 1080;
    size *= 0.9;
    winSize = size;
    
    window = glfwCreateWindow(winSize.x, winSize.y, "OpenGL", nullptr, nullptr);
    
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        throw std::runtime_error{"Failed to initialize GLAD"};
    }
    
    glViewport(0, 0, winSize.x, winSize.y);
    
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int w, int h) {
        std::cout << "window resized" << std::endl;
        glViewport(0, 0, w, h);
    });
}

Context::~Context()
{
    glfwDestroyWindow(window);
    window = nullptr;
    
    glfwTerminate();
}
