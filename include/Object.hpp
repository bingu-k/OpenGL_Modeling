#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Common.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texture_coord;
};

class Object
{
public:
    Object();
    // ~Object();

    void    CreatePlane(void);
    void    Draw(void);
private:
    GLuint  VAO {0};
    GLuint  VBO {0}, EBO {0};
    size_t  indexSize;

    void    BindBuffer(GLenum type, GLuint name, size_t dataSize, const void* data);
    void    SetAttrib(GLuint idx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
};

Object::Object()
{ glCreateVertexArrays(1, &this->VAO); };

// Object::~Object()
// {
//     if (VAO)
//         glDeleteVertexArrays(1, &VAO);
//     if (VBO)
//         glDeleteBuffers(1, &VBO);
//     if (EBO)
//         glDeleteBuffers(1, &EBO);
// }

void    Object::CreatePlane(void)
{
    std::vector<Vertex> vertices = {
        {{ 0.5f,  0.5f,  0.0f}, {0.8f, 0.4f, 0.3f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.0f}, {0.1f, 0.2f, 0.4f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.0f}, {0.2f, 0.4f, 0.3f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.0f}, {0.8f, 0.6f, 0.2f}, {0.0f, 1.0f}},
    };
    std::vector<GLuint> indices = {
        0, 1, 3,
        1, 2, 3
    };
    indexSize = indices.size();

    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
    
    glBindVertexArray(this->VAO);
    
    BindBuffer(GL_ARRAY_BUFFER, VBO, sizeof(Vertex) * vertices.size(), vertices.data());

	SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coord));

    BindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO, sizeof(GLuint) * indices.size(), indices.data());
    
    glBindVertexArray(0);
};

void    Object::Draw(void)
{
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indexSize, GL_UNSIGNED_INT, nullptr);
};

void    Object::BindBuffer(GLenum type, GLuint id, size_t dataSize, const void* data)
{
    glBindBuffer(type, id);
    glBufferData(type, dataSize, data, GL_STATIC_DRAW);
};

void    Object::SetAttrib(GLuint idx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer)
{
    glEnableVertexAttribArray(idx);
	glVertexAttribPointer(idx, size, type, normalized, stride, pointer);
};

#endif