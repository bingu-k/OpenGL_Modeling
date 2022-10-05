#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

#include <string>
#include <vector>

struct Vertex {
    glm::vec3   Position;
    glm::vec3   Normal;
    glm::vec2   TexCoords;
};

struct Texture {
    unsigned int    id;
    std::string     type;
    std::string     path;
};

class Mesh
{
public:
    std::vector<Vertex>   _vertices;
    std::vector<GLuint>   _indices;
    std::vector<Texture>  _textures;

    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
    // ~Mesh() <- 이거 만들어서 GL_INVAILED_OPERATION 뜸
    void    Draw(Shader& shader);
private:
    GLuint  VAO, VBO, EBO;
    void    setupMesh();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
    : _vertices(vertices), _indices(indices), _textures(textures)
{
    setupMesh();
};

void    Mesh::Draw(Shader& shader)
{
    GLuint  diffuseNr = 1;
    GLuint  specularNr = 1;

    for (unsigned int idx = 0; idx < this->_textures.size(); ++idx)
    {
        glActiveTexture(GL_TEXTURE0 + idx);
        std::string num;
        std::string name = this->_textures[idx].type;
        if (name == "texture_diffuse")
            num = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            num = std::to_string(specularNr++);

        shader.setInt(name + num, idx);
        glBindTexture(GL_TEXTURE_2D, this->_textures[idx].id);
    }

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
};

void    Mesh::setupMesh()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(Vertex), this->_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(GLuint), this->_indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
};

#endif // !MESH_HPP
