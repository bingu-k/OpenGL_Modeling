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
	static std::unique_ptr<Object>	CreatePlane(void);

    ~Object();
    void    Draw(void);
private:
    GLuint  VAO {0};
    GLuint  VBO {0}, EBO {0};
    size_t  indexSize;

    Object() {};
	void	init(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
    void    BindBuffer(GLenum type, GLuint name, size_t dataSize, const void* data);
    void    SetAttrib(GLuint idx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
};

Object::~Object()
{
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (EBO)
        glDeleteBuffers(1, &EBO);
}

std::unique_ptr<Object>	Object::CreatePlane(void)
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
	std::unique_ptr<Object>	plane = std::unique_ptr<Object>(new Object());
	plane->init(vertices, indices);
	return (std::move(plane));
};

void	Object::init(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
	glGenVertexArrays(1, &this->VAO);
    this->indexSize = indices.size();

    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    
    glBindVertexArray(this->VAO);
    
    BindBuffer(GL_ARRAY_BUFFER, this->VBO, sizeof(Vertex) * vertices.size(), vertices.data());

	SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coord));

    BindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO, sizeof(GLuint) * indices.size(), indices.data());
    
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