#pragma once

#include "Mesh.hpp"
#include <filesystem>

class aiNode;
class aiScene;
class aiMesh;

namespace obj
{
    class Model
    {
    public:
        Model(const std::filesystem::path& path);
        
        void draw(gl::Shader& shader) const;
        
    private:
        void processNode(aiNode& node, const aiScene& scene);
        Mesh processMesh(aiMesh& mesh, const aiScene& scene);
        
        std::vector<Mesh> meshes; ///< Children meshes.
        std::filesystem::path directory; ///< Where to load textures
    };
}