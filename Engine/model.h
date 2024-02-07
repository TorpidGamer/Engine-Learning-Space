#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/matrix.hpp>
#include <glm/gtx/transform.hpp>
#include <glad/glad.h>
#include "mesh.h"
#include "shader.h"
//This class is to make the meshes that are manually created and models that are imported uniform as it will halve the code we need
class Model {
public:
    std::vector<Texture> texturesLoaded;
    std::vector<Mesh> meshes;
    glm::vec3 position;

    Model(Mesh mesh);
    Model(std::string path);

    void Draw(Shader shader);
    std::vector<Texture> loadMatTexture(aiMaterial* mat, aiTextureType type, std::string typeName, std::string dir);
};