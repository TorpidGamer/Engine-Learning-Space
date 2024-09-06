#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>
#include <string>
#include <vector>
#include "Shader.h"
void LoadTextureManually(unsigned int* id, int width, int height, int channels, unsigned char* texData);
unsigned int LoadTextureFromFile(const char* path, const std::string& directory, bool recursive = true);

struct Vertex {
	glm::vec3 vertexPosition;
	glm::vec3 vertexNormal;
	glm::vec2 textureCoord;
	Vertex(glm::vec3 pos, glm::vec3 vertNorm, glm::vec2 textCoord);
};

struct Texture {
	unsigned int id;

	unsigned char* textureData;
	int channels;
	std::string type = "diffTex"; //type being if its diffuse specular etc

	//Manual section in case we want to make our own textures with code
	int width;
	int height;
	Texture(int texWidth, int texHeight, int numChannels, std::vector<unsigned char> texData);

	//Section for making textures out of saved images
	std::string path;
	Texture(std::string texPath);
	Texture(int _id, std::string _type, std::string _path);
};

struct Mesh {
	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 rotAxis = glm::vec3(1);
	std::vector<glm::vec3> edges, surfaceNormals;
	float rotationAmount = 0; 

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	glm::vec3 localSpacePosition;

	unsigned int VAO, VBO, EBO;
	bool Null = true;
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indi, std::vector<Texture> texes, bool genNormals = true);
	void calculateEdges();
};

void DrawMesh(Shader shader, Mesh* meshToDraw);