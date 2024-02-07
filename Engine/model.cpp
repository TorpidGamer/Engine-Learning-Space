#include "model.h"

Model::Model(Mesh mesh)
{
	meshes.push_back(mesh);
}

Model::Model(std::string path)
{
    Assimp::Importer importer;
    std::string textureDir;
    textureDir = path.substr(0, path.find_last_of('/'));
    std::cout << path << std::endl;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);
    if (!scene)
    {
        std::cout << "Unable to load object" << std::endl;
        return;
    }
    std::vector<Mesh> meshs;
    for (int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* curMesh = scene->mMeshes[m];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        for (int v = 0; v < curMesh->mNumVertices; v++)
        {
            aiVector3D vPos = curMesh->mVertices[v];
            aiVector3D vTex = curMesh->mTextureCoords[0][v];
            aiVector3D vNorm = curMesh->mNormals[v];
            glm::vec3 vertexPos = glm::vec3(vPos.x, vPos.y, vPos.z);
            glm::vec2 texCoord = glm::vec2(vTex.x, vTex.y);
            glm::vec3 normals = glm::vec3(vNorm.x, vNorm.y, vNorm.z);
            vertices.push_back(Vertex(vertexPos, normals, texCoord));
        }
        for (int f = 0; f < curMesh->mNumFaces; f++)
        {
            aiFace curFace = curMesh->mFaces[f];
            for (int i = 0; i < curFace.mNumIndices; i++)
            {
                indices.push_back(curFace.mIndices[i]);
            }
        }
        if (curMesh->mMaterialIndex > 0)
        {
            aiMaterial* material = scene->mMaterials[curMesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadMatTexture(material, aiTextureType_DIFFUSE, "diffTex", textureDir);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<Texture> specMaps = loadMatTexture(material, aiTextureType_SPECULAR, "specTex", textureDir);
            textures.insert(textures.end(), specMaps.begin(), specMaps.end());
        }
        meshes.push_back(Mesh(vertices, indices, textures, false));
    }
}

void Model::Draw(Shader shader)
{
    shader.use();
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].position = position;
        DrawMesh(shader, &meshes[i]);
    }
}

std::vector<Texture> Model::loadMatTexture(aiMaterial* mat, aiTextureType type, std::string typeName, std::string dir)
{

    std::vector<Texture> texturesVec;

    for (int i = 0; i < mat->GetTextureCount(type); i++)
    {

        aiString texPath;
        mat->GetTexture(type, i, &texPath);

        bool skip = false;

        for (int t = 0; t < texturesLoaded.size(); t++)
        {
            if (std::strcmp(texturesLoaded[t].path.data(), texPath.C_Str()) == 0)
            {
                texturesVec.push_back(texturesLoaded[t]);
                //cout << "Texture Duplicate: " << texturesLoaded[t].path << endl;
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            unsigned int id = LoadTextureFromFile(texPath.C_Str(), dir);
            Texture tex = Texture(id, typeName, texPath.C_Str());
            texturesVec.push_back(tex);
            texturesLoaded.push_back(tex);
        }
    }
    return texturesVec;
}