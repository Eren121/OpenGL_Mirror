#include "Triangle.hpp"
#include "Scene.hpp"

Triangle::Triangle(const obj::Vertex vertices[3])
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * 3, vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(obj::AttrVertex);
    gl::vertexAttribPointer(obj::AttrVertex, 3, GL_FLOAT, &obj::Vertex::pos);
    
    glEnableVertexAttribArray(obj::AttrTextCoords);
    gl::vertexAttribPointer(obj::AttrTextCoords, 2, GL_FLOAT, &obj::Vertex::texCoords);
    
    glBindVertexArray(0);
}

void Triangle::draw()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
