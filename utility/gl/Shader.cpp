#include "Shader.hpp"
#include <utility/io.hpp>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace gl
{
    void Shader::load(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        gl::raii::Shader vert(GL_VERTEX_SHADER);
        gl::raii::Shader frag(GL_FRAGMENT_SHADER);
        
        gl::compileShader(vert, vertexSrc);
        gl::compileShader(frag, fragmentSrc);
        gl::linkShadersToProgram(m_program, vert, frag);
        
        // At the end of the scope the GL::Shader will be deleted,
        // it's fine because they are no more necessary
    }
    
    void Shader::load(const std::filesystem::path& vert, const std::filesystem::path& frag)
    {
        load(io::readAll(vert), io::readAll(frag));
    }
    
    void Shader::bind(const Shader *shader)
    {
        if (shader)
        {
            glUseProgram(shader->m_program);
        } else
        {
            glUseProgram(0);
        }
    }
    
    void Shader::setUniform(const std::string& name, const glm::mat4& value)
    {
        Shader::bind(this);
        glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, false, glm::value_ptr(value));
    }
    
    void Shader::setUniform(const std::string& name, int value)
    {
        Shader::bind(this);
        glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
    }
    
    void Shader::setUniform(const std::string& name, unsigned int value)
    {
        setUniform(name, static_cast<int>(value));
    }
    
    void Shader::setUniform(const std::string& name, float value)
    {
        Shader::bind(this);
        glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
    }
    
    void Shader::setUniform(const std::string& name, const glm::vec4& value)
    {
        Shader::bind(this);
        glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
    }
    
    void Shader::setUniform(const std::string& name, const glm::vec3& value)
    {
        Shader::bind(this);
        glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
    }
}