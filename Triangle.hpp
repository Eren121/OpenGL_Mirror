#pragma once

#include "Scene.hpp"
#include <glm/vec3.hpp>
#include <utility/gl/gl.hpp>

class Triangle
{
public:
    Triangle(const obj::Vertex vertices[3]);
    
    void draw();
    
private:
    gl::raii::Buffer vbo;
    gl::raii::VertexArray vao;
};

