#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include after glad otherwise compilation error
#include <glm/vec2.hpp>

/// @brief Wraps an OpenGL context and a window
class Context
{
public:
    Context();
    ~Context();
    
    GLFWwindow *window{nullptr};
    glm::ivec2 winSize;
};

