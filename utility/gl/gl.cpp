#include "gl.hpp"
#include <utility/unused.hpp>
#include <cassert>
#include <iostream>
#include <GL/gl.h>

namespace gl
{
    namespace raii
    {
        Shader::Shader(GLenum type)
        {
            assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER);
            
            id = glCreateShader(type);
            if (!id)
            {
                std::cerr << "Failed to create a shader" << std::endl;
            }
        }
    
        Shader::~Shader()
        {
            glDeleteShader(id);
        }
    
        Program::Program()
        {
            id = glCreateProgram();
            if (!id)
            {
                std::cerr << "Failed to create a shader program" << std::endl;
            }
        }
    
        Program::~Program()
        {
            glDeleteProgram(id);
        }
    
        Buffer::Buffer()
        {
            glGenBuffers(1, &id);
        }
    
        Buffer::~Buffer()
        {
            glDeleteBuffers(1, &id);
        }
    
        VertexArray::VertexArray()
        {
            glGenVertexArrays(1, &id);
        }
    
        VertexArray::~VertexArray()
        {
            glDeleteVertexArrays(1, &id);
        }
    
        Texture::Texture()
        {
            glGenTextures(1, &id);
        }
    
        Texture::~Texture()
        {
            glDeleteTextures(1, &id);
        }
    
        Framebuffer::Framebuffer()
        {
            glGenFramebuffers(1, &id);
        }
    
        Framebuffer::~Framebuffer()
        {
            glDeleteFramebuffers(1, &id);
        }
    
        Renderbuffer::Renderbuffer()
        {
            glGenRenderbuffers(1, &id);
        }
    
        Renderbuffer::~Renderbuffer()
        {
            glDeleteRenderbuffers(1, &id);
        }
    }
    
    std::string getProgramInfoLog(unsigned int programID)
    {
        int len; // Count of characters in the log message (including null-terminating char)
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &len);
        
        std::vector<char> log(len, '\0');
        glGetProgramInfoLog(programID, len, nullptr, &log[0]);
        
        return std::string{log.begin(), log.end()};
    }
    
    std::string getShaderInfoLog(unsigned int shaderID)
    {
        int len; // Count of characters in the log message (including null-terminating char)
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
        
        std::vector<char> log(len, '\0');
        glGetShaderInfoLog(shaderID, len, nullptr, &log[0]);
        
        return std::string{log.begin(), log.end()};
    }
    
    void compileShader(unsigned int shaderID, const std::string& source)
    {
        const char* c_source = source.c_str();
        glShaderSource(shaderID, 1, &c_source, nullptr); // The source is not compiled, just copied into the shader object at this time
        
        glCompileShader(shaderID);
        
        int status;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
        
        if(!status)
        {
            std::cerr << "Failed to compile the shader: " << getShaderInfoLog(shaderID) << std::endl;
        }
    }
    
    void linkShadersToProgram(unsigned int programID, unsigned int fragmentShaderID, unsigned int vertexShaderID)
    {
        glAttachShader(programID, vertexShaderID);
        glAttachShader(programID, fragmentShaderID);
        glLinkProgram(programID);
        
        int success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            std::cerr << "Failed to link the program: " << getProgramInfoLog(programID) << std::endl;
        }
    }
    
    void enableDebugging(bool throwOnError)
    {
    }
}