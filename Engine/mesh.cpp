#include"mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
unsigned int LoadTextureFromFile(const char* path, const std::string& directory, bool recursive)
{
	std::string filename = std::string(path);
	size_t position;

	if (!recursive)
	{
		position = filename.find_last_of("\\");
		filename.erase(0, position);
		filename.erase(filename.begin());
	}

	filename = directory + '/' + filename;
	std::cout << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (data)
	{
		GLenum format;
		if (channels == 1)
			format = GL_RED;
		else if (channels == 3)
			format = GL_RGB;
		else if (channels == 4)
			format = GL_RGBA;
		else
		{
			std::cout << "Unusable number of channels. Defaulting to 3" << std::endl;
			format = GL_RGB;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << ", " << recursive << std::endl;
		if (recursive)
			LoadTextureFromFile(path, directory, false);
	}
	stbi_image_free(data);
	return textureID;
}
void LoadTextureManually(unsigned int* id, int width, int height, int channels, unsigned char* texData)
{
	glGenTextures(1, id);
	glBindTexture(GL_TEXTURE_2D, *id);
	// set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format;

	if (channels == 4)
	{
		format = GL_RGBA;
	}
	else if (channels == 3)
	{
		format = GL_RGB;
	}
	else if (channels == 1)
	{
		format = GL_RED;
	}
	else
	{
		std::cout << "Unsupported number of channels for texture: " << channels << std::endl;
		return;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);
}
void DrawMesh(Shader shader, Mesh* meshToDraw)
{
	for (int j = 0; j < meshToDraw->textures.size(); j++)
	{
		glActiveTexture(GL_TEXTURE0 + j);
		int diffuseNum = 1;
		int specularNum = 1;

		std::string type = meshToDraw->textures[j].type;
		std::string index;
		if (type == "diffTex")
			index = std::to_string(diffuseNum++);
		else if (type == "specTex")
			index = std::to_string(specularNum++);
		shader.setInt(type + index, meshToDraw->textures[j].id);
		glBindTexture(GL_TEXTURE_2D, meshToDraw->textures[j].id);
	}
	shader.setInt("texNum", meshToDraw->textures.size());
	glActiveTexture(GL_TEXTURE0);
	glm::mat4 modelMat = glm::mat4(1.f);
	modelMat = glm::scale(modelMat, meshToDraw->scale);
	modelMat = glm::rotate(modelMat, glm::radians(meshToDraw->rotationAmount), meshToDraw->rotAxis);
	modelMat = glm::translate(modelMat, meshToDraw->position);
	shader.setMat4("model", modelMat);

	glBindVertexArray(meshToDraw->VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(meshToDraw->indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Vertex::Vertex(glm::vec3 pos, glm::vec3 vertNorm = glm::vec3(0), glm::vec2 textCoord = glm::vec2(0))
{
	vertexPosition = pos;
	vertexNormal = vertNorm;
	textureCoord = textCoord;
}
Texture::Texture(std::string texPath)
{
	path = texPath;
	glGenTextures(1, &id);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data)
	{
		GLenum format;
		if (channels == 1)
			format = GL_RED;
		else if (channels == 3)
			format = GL_RGB;
		else if (channels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
}
Texture::Texture(int _id, std::string _type, std::string _path)
{
	id = _id;
	type = _type;
	path = _path;
}
Texture::Texture(int texWidth, int texHeight, int numChannels, std::vector<unsigned char> texData)
{
	width = texWidth;
	height = texHeight;
	channels = numChannels;
	textureData = texData.data();
	LoadTextureManually(&id, width, height, channels, textureData);
}

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indi, std::vector<Texture> texes, bool genNormals)
{
	vertices = verts;
	indices = indi;
	textures = texes;

	//Calculate normals for the vertices
	if (genNormals)
	{
		int triangleCount = indices.size() / 3;
		for (int i = 0; i < triangleCount; i++)
		{
			int triangleIndex = i * 3;
			int vertexIndexA = indices[triangleIndex];
			int vertexIndexB = indices[triangleIndex + 1];
			int vertexIndexC = indices[triangleIndex + 2];

			glm::vec3 pointA = vertices[vertexIndexA].vertexPosition;
			glm::vec3 pointB = vertices[vertexIndexB].vertexPosition;
			glm::vec3 pointC = vertices[vertexIndexC].vertexPosition;

			glm::vec3 sideAB = pointB - pointA;
			glm::vec3 sideAC = pointC - pointA;

			glm::vec3 triangleNormal = glm::normalize(glm::cross(sideAB, sideAC));
			vertices[vertexIndexA].vertexNormal = triangleNormal;
			vertices[vertexIndexB].vertexNormal = triangleNormal;
			vertices[vertexIndexC].vertexNormal = triangleNormal;
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//a Structs memory layouts are sequential which means giving a pointer to a struct will lay all the bytes out meaning we dont need to calculate how many bytes we need or the layout
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//vertex attribute pointers
	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertexNormal));
	//Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoord));

	glBindVertexArray(0);
}