#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "Shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

unsigned int	TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
	std::vector<Texture>	textures_loaded;
	std::vector<Mesh>		meshes;
	std::string				directory;
	bool					gammaCorrection;

	Model(std::string path);
	~Model();

	void	Draw(Shader& shader);

private:
	void	loadModel(std::string path);
	void	processNode(aiNode* node, const aiScene* scene);
	Mesh	processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture>	loadMaterialTextures(aiMaterial* mat, aiTextureType type,
												std::string typename);
};

Model::Model(std::string path)
{
	loadModel(path);
};

Model::~Model()
{};

void	Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
		meshes[i].Draw(shader);
};

void	Model::loadModel(std::string path)
{
	Assimp::Importer	importer;

	// Pre-processing
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return ;
	}
	this->directory = path.substr(0, path.find_last_of('\\'));

	processNode(scene->mRootNode, scene);
};

void	Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int idx = 0; idx < node->mNumMeshes; ++idx)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[idx]];
		this->meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int idx = 0; idx < node->mNumChildren; ++idx)
		processNode(node->mChildren[idx], scene);
};

Mesh	Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex>		vertices;
	std::vector<GLuint>		indices;
	std::vector<Texture>	textures;

	for (unsigned int idx = 0; idx < mesh->mNumVertices; ++idx)
	{
		Vertex	vertex;

		glm::vec3 vector;
		vector.x = mesh->mVertices[idx].x;
		vector.y = mesh->mVertices[idx].y;
		vector.z = mesh->mVertices[idx].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[idx].x;
		vector.y = mesh->mNormals[idx].y;
		vector.z = mesh->mNormals[idx].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][idx].x;
			vec.y = mesh->mTextureCoords[0][idx].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	for (unsigned int out_idx = 0; out_idx < mesh->mNumFaces; ++out_idx)
	{
		aiFace	face = mesh->mFaces[out_idx];
		for (unsigned int in_idx = 0; in_idx < face.mNumIndices; ++in_idx)
			indices.push_back(face.mIndices[in_idx]);
	}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture>	diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture>	specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	return (Mesh(vertices, indices, textures));
};

std::vector<Texture>	Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture>	textures;
	for (unsigned int out_idx = 0; out_idx < mat->GetTextureCount(type); ++out_idx)
	{
		aiString	str;
		mat->GetTexture(type, out_idx, &str);
		bool	skip = false;
		for (unsigned int in_idx = 0; in_idx < textures_loaded.size(); ++in_idx)
		{
			if (std::strcmp(textures_loaded[in_idx].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[in_idx]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture	texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return (textures);
};

unsigned int	TextureFromFile(const char* path, const std::string& directory)
{
	std::string	filename = std::string(path);
	filename = directory + '\\' + filename;

	GLuint	textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return (textureID);
};

#endif // !MODEL_HPP