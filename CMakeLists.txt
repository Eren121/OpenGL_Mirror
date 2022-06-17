cmake_minimum_required(VERSION 3.17)
project(OpenGL_OBJ)

set(CMAKE_CXX_STANDARD 20)

set(GLAD_SRC glad/src/glad.c)

set(IMGUI_SRC
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp)
include_directories(thirdparty/imgui/backends thirdparty/imgui)

set(UTILITY_SRC
    utility/offset_of.hpp
    utility/gl/gl.cpp utility/gl/gl.cpp
    utility/gl/Shader.cpp utility/gl/Shader.hpp
    utility/gl/Texture.cpp utility/gl/Texture.hpp
    utility/gl/stb_image.h utility/io.cpp utility/io.hpp
    utility/time/Clock.cpp
    utility/time/Clock.hpp
    utility/time/FPSCounter.cpp
    utility/time/FPSCounter.hpp
    utility/time/Time.cpp
    utility/time/Time.hpp
    utility/time/Timer.cpp
    utility/time/Timer.hpp
    Model.cpp Model.hpp utility/conversion.hpp Scene.cpp Scene.hpp Triangle.cpp Triangle.hpp)

add_executable(OpenGL_OBJ main.cpp Mesh.cpp Mesh.hpp Context.cpp Context.hpp ${GLAD_SRC} ${UTILITY_SRC} ${IMGUI_SRC})
target_link_libraries(OpenGL_OBJ glfw dl assimp)

target_include_directories(OpenGL_OBJ PRIVATE .)
target_include_directories(OpenGL_OBJ PRIVATE glad/include)