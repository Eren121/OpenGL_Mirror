#pragma once

#include <utility/gl/gl.hpp>
#include <utility/gl/Shader.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace obj
{
    struct Vertex
    {
        glm::vec3 pos{0}; ///< Position
        glm::vec2 texCoords{0}; ////< Texture coordinates
        glm::vec3 nor{0}; ///< Normal
    };
    
    enum Attribute
    {
        AttrVertex = 0,
        AttrNormal = 1,
        AttrTextCoords = 2
    };
    
    struct Material
    {
        gl::Texture diffuseTexture;
        glm::vec4 diffuseColor{1};
    };
    
    class Mesh
    {
    public:
        using Vertices = std::vector<Vertex>;
        using Indices = std::vector<unsigned int>;
        
        Mesh(Vertices vertices, Indices indices, Material material);
        
        void draw(gl::Shader& shader) const;
        
    private:
        void init();
        
        Vertices vertices;
        Indices indices;
        Material material;
        
        gl::raii::VertexArray vao;
        gl::raii::Buffer vbo, ebo;
    };
}