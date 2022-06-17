#include "Mesh.hpp"

namespace obj
{
    
    Mesh::Mesh(Mesh::Vertices vertices, Mesh::Indices indices, Material material)
        : vertices(std::move(vertices)), indices(std::move(indices)), material(std::move(material))
    {
        init();
    }
    
    void Mesh::init()
    {
        glBindVertexArray(vao);
    
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        gl::bufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);
    
        glEnableVertexAttribArray(AttrVertex);
        gl::vertexAttribPointer(AttrVertex, 3, GL_FLOAT, &Vertex::pos);
    
        glEnableVertexAttribArray(AttrNormal);
        gl::vertexAttribPointer(AttrNormal, 3, GL_FLOAT, &Vertex::nor);
    
        glEnableVertexAttribArray(AttrTextCoords);
        gl::vertexAttribPointer(AttrTextCoords, 2, GL_FLOAT, &Vertex::texCoords);
    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        gl::bufferData(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
    
        glBindVertexArray(0);
    }
    
    void Mesh::draw(gl::Shader& shader) const
    {
        glActiveTexture(GL_TEXTURE1);
        gl::Texture::bind(&material.diffuseTexture);
        glActiveTexture(GL_TEXTURE0);
        
        shader.setUniform("u_DiffuseColor", material.diffuseColor);
        
        glBindVertexArray(vao); // vao also remember of ebo that's why we don't need to bind the ebo
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}