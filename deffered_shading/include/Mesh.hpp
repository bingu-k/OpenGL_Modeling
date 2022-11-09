#ifndef MESH_HPP
#define MESH_HPP

#include "Common.hpp"
#include "Program.hpp"

struct mVertex
{
    glm::vec3   position;
    glm::vec3   normal;
    glm::vec2   texCoords;
    glm::vec3   tangent;
};

struct mTexture
{
    GLuint      id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<mVertex>    vertices;
    std::vector<GLuint>     indices;
    std::vector<mTexture>   textures;

    Mesh(std::vector<mVertex> vertices, std::vector<unsigned int> indices, std::vector<mTexture> textures);
    ~Mesh();
    void    Draw(Program* program);
private:
    GLuint  VAO, VBO, EBO;

    void    setupMesh(void);
};

Mesh::Mesh(std::vector<mVertex> vertices, std::vector<unsigned int> indices, std::vector<mTexture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
};

Mesh::~Mesh()
{
    // if (VAO)
    //     glDeleteVertexArrays(1, &VAO);
    // if (VBO)
    //     glDeleteBuffers(1, &VBO);
    // if (EBO)
    //     glDeleteBuffers(1, &EBO);
};

void    Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(mVertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mVertex), (void*)offsetof(mVertex, position));
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mVertex), (void*)offsetof(mVertex, normal));
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mVertex), (void*)offsetof(mVertex, texCoords));
    glEnableVertexAttribArray(3);	
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(mVertex), (void*)offsetof(mVertex, tangent));

    glBindVertexArray(0);
};

void    Mesh::Draw(Program* program) 
{
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        program->setUniform((int)i, ("material." + textures[i].type).c_str());
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
};

#endif