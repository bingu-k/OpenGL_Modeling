#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Common.hpp"

struct Vertex
{
    glm::vec3   position;
    glm::vec3   normal;
    glm::vec2   texture_coord;
    glm::vec3   tangent;
    glm::vec3   bitangent;
};

struct Screen
{
    glm::vec3   position;
    glm::vec3   color;
    glm::vec2   texture_coord;
};

void        CalTangentSpace(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

class Object
{
public:
	static std::unique_ptr<Object>	CreatePlane(void);
	static std::unique_ptr<Object>	CreateBox(void);
	static std::unique_ptr<Object>	CreateScreen(void);

    ~Object();
    void    Draw(void);
private:
    GLuint  VAO {0};
    GLuint  VBO {0}, EBO {0};
    size_t  indexSize;

    Object() {};
	void	init(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
	void	init(std::vector<Screen>& vertices, std::vector<GLuint>& indices);
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
        {{ 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {}},
        {{ 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {}},
        {{-0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {}},
        {{-0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {}},
    };
    std::vector<GLuint> indices = {
        0, 1, 3,
        1, 2, 3
    };
	std::unique_ptr<Object>	plane = std::unique_ptr<Object>(new Object());
    CalTangentSpace(vertices, indices);
    plane->init(vertices, indices);
	return (std::move(plane));
};

std::unique_ptr<Object>	Object::CreateBox(void)
{
    std::vector<Vertex> vertices = {
        {{ -0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {}},
        {{  0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {}},
        {{  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {}},
        {{ -0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {}},

        {{ -0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {}},
        {{  0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {}},
        {{  0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {}},
        {{ -0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {}},

        {{ -0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {}},
        {{ -0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {}},
        {{ -0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {}},
        {{ -0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {}},
        
        {{  0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {}},
        {{  0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {}},
        {{  0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {}},
        {{  0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {}},
        
        {{ -0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {}},
        {{  0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {}},
        {{  0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {}},
        {{ -0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {}},
        
        {{ -0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {}},
        {{  0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {}},
        {{  0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {}},
        {{ -0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {}}
    };
    std::vector<GLuint> indices = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23
    };

	std::unique_ptr<Object>	plane = std::unique_ptr<Object>(new Object());
    CalTangentSpace(vertices, indices);
    plane->init(vertices, indices);
	return (std::move(plane));
};


std::unique_ptr<Object>	Object::CreateScreen(void)
{
    std::vector<Screen> vertices = {
        {{ 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
    };
    std::vector<GLuint> indices = {
        0, 1, 3,
        1, 2, 3
    };
	std::unique_ptr<Object> screen = std::unique_ptr<Object>(new Object());
    screen->init(vertices, indices);
	return (std::move(screen));
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
	SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coord));
	SetAttrib(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    BindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO, sizeof(GLuint) * indices.size(), indices.data());
    
    glBindVertexArray(0);
};

void	Object::init(std::vector<Screen>& vertices, std::vector<GLuint>& indices)
{
	glGenVertexArrays(1, &this->VAO);
    this->indexSize = indices.size();

    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);
    
    glBindVertexArray(this->VAO);
    
    BindBuffer(GL_ARRAY_BUFFER, this->VBO, sizeof(Screen) * vertices.size(), vertices.data());

	SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Screen), (void*)offsetof(Screen, position));
	SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Screen), (void*)offsetof(Screen, color));
	SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Screen), (void*)offsetof(Screen, texture_coord));
	
    BindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO, sizeof(GLuint) * indices.size(), indices.data());
    
    glBindVertexArray(0);
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

void    Object::Draw(void)
{
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indexSize, GL_UNSIGNED_INT, nullptr);
};

glm::vec3   ComputeTangentSpace(std::vector<Vertex>& vertices, GLuint i, GLuint j, GLuint k)
{
    glm::vec3   edge1 = vertices[j].position - vertices[i].position;
    glm::vec3   edge2 = vertices[k].position - vertices[i].position;
    glm::vec2   uv1 = vertices[j].texture_coord - vertices[i].texture_coord;
    glm::vec2   uv2 = vertices[k].texture_coord - vertices[i].texture_coord;
    float       det = uv1.x * uv2.y - uv1.y * uv2.x;

    if (det != 0.0f)
    {
        float   invDet = 1.0f / det;
        return (invDet * (uv2.y * edge1 - uv1.y * edge2));
    }
    else
        return (glm::vec3(0.0f, 0.0f, 0.0f));
};


void    CalTangentSpace(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    std::vector<glm::vec3>  tangents;
    tangents.resize(vertices.size());
    memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        tangents[indices[i]] += ComputeTangentSpace(vertices, indices[i], indices[i + 1], indices[i + 2]);
        tangents[indices[i + 1]] += ComputeTangentSpace(vertices, indices[i + 1], indices[i + 2], indices[i]);
        tangents[indices[i + 2]] += ComputeTangentSpace(vertices, indices[i + 2], indices[i], indices[i + 1]);
    }

    for (size_t i = 0; i < vertices.size(); ++i)
        vertices[i].tangent = glm::normalize(tangents[i]);
};

#endif