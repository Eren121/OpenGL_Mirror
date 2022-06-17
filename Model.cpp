#include "Model.hpp"
#include <utility/conversion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace obj
{
    Model::Model(const std::filesystem::path& path)
        : directory(path.parent_path())
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Failed to load the model " << path << ": " << importer.GetErrorString() << std::endl;
        }
        
        processNode(*scene->mRootNode, *scene);
    }
    
    void Model::draw(gl::Shader& shader) const
    {
        for(const Mesh& mesh : meshes)
        {
            mesh.draw(shader);
        }
    }
    
    void Model::processNode(aiNode& node, const aiScene& scene)
    {
        for(unsigned int i = 0; i < node.mNumMeshes; ++i)
        {
            aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
            meshes.push_back(processMesh(*mesh, scene));
        }
        
        for(unsigned int i = 0; i < node.mNumChildren; ++i)
        {
            processNode(*node.mChildren[i], scene);
        }
    }
    
    Mesh Model::processMesh(aiMesh& mesh, const aiScene& scene)
    {
        Mesh::Vertices vertices;
        Mesh::Indices indices;
        Material material;
        
        // Process the vertices
        for(unsigned int i = 0; i < mesh.mNumVertices; ++i)
        {
            Vertex v;
            v.pos = convert_vec3<glm::vec3>(mesh.mVertices[i]);
            v.nor = convert_vec3<glm::vec3>(mesh.mNormals[i]);
            
            if(mesh.mTextureCoords[0]) // Check the mesh contains at least one texture
            {
                v.texCoords = convert_vec2<glm::vec2>(mesh.mTextureCoords[0][i]);
            }
            
            vertices.push_back(v);
        }
        
        // Process the indices: [face] -> [indice]
        for(unsigned int i = 0; i < mesh.mNumFaces; ++i)
        {
            // Always triangles because aiProcess_Triangulate
            const aiFace& face = mesh.mFaces[i];
            
            for(unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
        
        // Process the textures
        if(mesh.mMaterialIndex >= 0)
        {
            const aiMaterial *const aiM = scene.mMaterials[mesh.mMaterialIndex];
            
            aiString path;
            aiReturn status{aiM->GetTexture(aiTextureType_DIFFUSE, 0, &path)};
            if(status == aiReturn_SUCCESS)
            {
                const std::filesystem::path texPath = directory / path.C_Str();
                std::cout << "Loading texture " << texPath << std::endl;
                material.diffuseTexture.load(texPath);
            }
            else
            {
                std::cerr << "Failed to get a aiM texture (aiReturn=" <<  status << ")" << std::endl;
            }
    
            
            aiColor3D diffuseColor;
            status = aiM->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            if(status == aiReturn_SUCCESS)
            {
                glm::vec4 color;
                color.r = diffuseColor.r;
                color.g = diffuseColor.g;
                color.b = diffuseColor.b;
                color.a = 1;
    
                std::cout << "Loading material with diffuse color = " << to_string(color) << std::endl;
                material.diffuseColor = color;
            }
            else
            {
                std::cerr << "Failed to get the diffuse color" << std::endl;
                material.diffuseColor = glm::vec4{1};
            }
        }
        else
        {
            // White diffuse texture by default
            material.diffuseTexture.load({1, 1}, glm::vec4{1});
        }
        
        return Mesh{vertices, indices, std::move(material)};
    }
}