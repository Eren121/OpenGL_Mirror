#pragma once

#include <string>
#include <vector>
#include <utility>
#include <utility/offset_of.hpp>
#include <glad/glad.h> // Change this include depending to your OpenGL loader

/// @brief OpenGL utilities
namespace gl
{
    namespace raii
    {
        /// @brief Generic OpenGL object RAII
        template<typename T = unsigned int, T invalid_value = T(0)>
        struct GLObject
        {
            using value_type = T;
        
            GLObject() = default;
        
            virtual ~GLObject()
            { id = invalid_value; }
        
            GLObject& operator=(GLObject&& rhs) noexcept
            {
                swap(*this, rhs);
                return *this;
            }
        
            explicit GLObject(GLObject&& rhs)
            {
                swap(*this, rhs);
            }
        
            GLObject& operator=(const GLObject&) = delete;
        
            GLObject(const GLObject&) = delete;
        
            /// @brief Non-explicit because opengl ID are normally really only integers.
            operator value_type() const
            { return id; }
        
            value_type id{invalid_value};
        
            /// @remarks We have to define this function otherwise move constructors will go into infinite recursion
            friend void swap(GLObject& a, GLObject& b)
            {
                std::swap(a.id, b.id);
            }
        };
    
        struct Shader : GLObject<>
        {
            Shader(GLenum type);
        
            ~Shader() override;
        
            Shader(Shader&&) = default;
        
            Shader& operator=(Shader&&) = default;
        };
    
        struct Program : GLObject<>
        {
            Program();
        
            ~Program() override;
        
            Program(Program&&) = default;
        
            Program& operator=(Program&&) = default;
        };
    
        struct Buffer : GLObject<>
        {
            Buffer();
        
            ~Buffer() override;
        
            Buffer(Buffer&&) = default;
        
            Buffer& operator=(Buffer&&) = default;
        };
    
        struct Texture : GLObject<>
        {
            Texture();
        
            ~Texture() override;
        
            Texture(Texture&&) = default;
        
            Texture& operator=(Texture&&) = default;
        };
    
    
        struct VertexArray : GLObject<>
        {
            VertexArray();
        
            ~VertexArray() override;
        
            VertexArray(VertexArray&&) = default;
        
            VertexArray& operator=(VertexArray&&) = default;
        };
        
        struct Framebuffer : GLObject<>
        {
            Framebuffer();
            ~Framebuffer() override;
            Framebuffer(Framebuffer&&) = default;
            Framebuffer& operator=(Framebuffer&&) = default;
        };
        
        struct Renderbuffer : GLObject<>
        {
            Renderbuffer();
            ~Renderbuffer() override;
            Renderbuffer(Renderbuffer&&) = default;
            Renderbuffer& operator=(Renderbuffer&&) = default;
        };
    }
    
    /// @brief Simpler glBufferData() for C++
    template<typename T>
    void bufferData(GLenum target, const std::vector<T>& buffer, GLenum usage)
    {
        glBufferData(target, buffer.size() * sizeof(buffer[0]), buffer.data(), usage);
    }
    
    /// @brief Simpler glVertexAttribPointer for C++
    /// @details Because it can be tricky in C++ to use stride, because members are not necessarily packed.
    template<typename Class, typename FieldType>
    void vertexAttribPointer(GLuint index, GLint size, GLenum type, FieldType(Class::*field))
    {
        // Compute the offset of the member
        // offset is C and doesn't works with templates
        // Taking the address will not dereference, not causing segfault.

        glVertexAttribPointer(index, size, type, GL_FALSE, sizeof(Class), reinterpret_cast<const void*>(offset_of(field)));
    }
    
    
    /// @returns Info log of a Shader.
    std::string getShaderInfoLog(unsigned int shaderID);
    
    /// @returns Info log of a Program.
    std::string getProgramInfoLog(unsigned int programID);
    
    /// @brief Compiles a string into a shader
    void compileShader(unsigned int shaderID, const std::string& source);
    
    /// @brief Links compiled shaders to a program
    void linkShadersToProgram(unsigned int programID, unsigned int fragmentShaderID, unsigned int vertexShaderID);
    
    
    void enableDebugging(bool throwOnError = false);
};

