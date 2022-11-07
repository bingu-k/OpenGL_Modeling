#ifndef MATHOP_HPP
#define MATHOP_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

class MathOP
{
public:
    static glm::mat4   lookAt(glm::vec3& pos, glm::vec3& dir, glm::vec3& up);

    static glm::mat4   perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);
    static glm::mat4   ortho(GLfloat left, GLfloat right,
                            GLfloat bottom, GLfloat top,
                            GLfloat near, GLfloat far);

    static glm::mat4   rotate(glm::mat4& matrix, const GLfloat& angle, glm::vec3& axis);
    static glm::mat4   translate(glm::mat4& matrix, glm::vec3& delta);
    static glm::mat4   scale(glm::mat4& matrix, glm::vec3& delta);

    static void        testing(void);
    static void        printMatrixs(glm::mat4& matrix1, glm::mat4& matrix2);
private:
    MathOP() {};
    ~MathOP() {};
};

glm::mat4   MathOP::lookAt(glm::vec3& pos, glm::vec3& front, glm::vec3& up)
{
    glm::vec3   axisZ = glm::normalize(front - pos);
    glm::vec3   axisX = glm::normalize(glm::cross(axisZ, up));
    glm::vec3   axisY = glm::normalize(glm::cross(axisX, axisZ));
    glm::mat4   rotateMatrix{
        axisX.x,    axisY.x,    -axisZ.x,    0.0f,
        axisX.y,    axisY.y,    -axisZ.y,    0.0f,
        axisX.z,    axisY.z,    -axisZ.z,    0.0f,
        0.0f,       0.0f,       0.0f,       1.0f
    };
    glm::mat4   tranlateMatrix = MathOP::translate(glm::mat4(1.0f), -pos);
    return (rotateMatrix * tranlateMatrix);
};

glm::mat4   MathOP::perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far)
{
    GLfloat top = near * tan(fovy / 2);
    GLfloat right = top * aspect;
    glm::mat4   result{
        near / right,   0,          0,                                  0,
        0,              near/top,   0,                                  0,
        0,              0,          (near + far) / (near - far),        -1,
        0,              0,          (2 * near * far) / (near - far),    0
    };
    return (result);
};
glm::mat4   MathOP::ortho(GLfloat left, GLfloat right,
                        GLfloat bottom, GLfloat top,
                        GLfloat near, GLfloat far)
{
    glm::mat4   scaleMatrix = MathOP::scale(glm::mat4(1.0f),
                                    glm::vec3(2 / (right - left), 2 / (top - bottom), -2 / (far - near)));
    glm::mat4   tranlateMatrix = MathOP::translate(glm::mat4(1.0f),
                                        glm::vec3(-(right + left) / 2, -(top + bottom) / 2, (far + near) / 2));
    return (scaleMatrix * tranlateMatrix);
};

// Transform Matrix
glm::mat4   MathOP::rotate(glm::mat4& matrix, const GLfloat& angle, glm::vec3& axis)
{
    glm::vec3   n = glm::normalize(axis);
    float       c = cos(angle);
    float       s = sin(angle);
    glm::mat3   k = { 0.0f,     -n[2],  n[1]
                    , n[2],     0.0f,   -n[0]
                    , -n[1],    n[0],   0.0f};
    glm::mat3   result = ((1.0f - c) * k * k) + (s * k) + glm::mat3(1.0f);
    glm::mat4   transform{
        result[0][0], result[1][0], result[2][0], 0.0f, 
        result[0][1], result[1][1], result[2][1], 0.0f, 
        result[0][2], result[1][2], result[2][2], 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    return (matrix * transform);
};
glm::mat4   MathOP::translate(glm::mat4& matrix, glm::vec3& delta)
{
    glm::mat4   transform{
        1.0f,       0.0f,       0.0f,       0.0f,
        0.0f,       1.0f,       0.0f,       0.0f,
        0.0f,       0.0f,       1.0f,       0.0f,
        delta[0],   delta[1],   delta[2],   1.0f
    };
    return (matrix * transform);
};
glm::mat4   MathOP::scale(glm::mat4& matrix, glm::vec3& delta)
{
    glm::mat4   transform{
        delta[0],   0.0f,       0.0f,       0.0f,
        0.0f,       delta[1],   0.0f,       0.0f,
        0.0f,       0.0f,       delta[2],   0.0f,
        0.0f,       0.0f,       0.0f,       1.0f
    };
    return (matrix * transform);
};


void    MathOP::printMatrixs(glm::mat4& matrix1, glm::mat4& matrix2)
{
    std::cout << "-----------------------------------------------------" << std::endl;
    for (unsigned int i = 0; i < 4; ++i)
    {
        std::cout << matrix1[i][0] << " "
                << matrix1[i][1] << " "
                << matrix1[i][2] << " "
                << matrix1[i][3] << std::endl;
    }
    std::cout << std::endl;    
    for (unsigned int i = 0; i < 4; ++i)
    {
        std::cout << matrix2[i][0] << " "
                << matrix2[i][1] << " "
                << matrix2[i][2] << " "
                << matrix2[i][3] << std::endl;
    }
    std::cout << "-----------------------------------------------------" << std::endl;
};
void    MathOP::testing(void)
{
    glm::mat4   basic(1.0f);
    // Scale
    {
        std::cout << "\tScale" << std::endl;
        glm::vec3   delta{1.5f, 1.5f, 1.5f};
        MathOP::printMatrixs(glm::scale(basic, delta), MathOP::scale(basic, delta));
        delta = glm::vec3{0.5f, 0.5f, 0.5f};
        MathOP::printMatrixs(glm::scale(basic, delta), MathOP::scale(basic, delta));
        delta = glm::vec3{0.0f, 0.0f, 0.0f};
        MathOP::printMatrixs(glm::scale(basic, delta), MathOP::scale(basic, delta));
        delta = glm::vec3{2.0f, 3.0f, 4.0f};
        MathOP::printMatrixs(glm::scale(basic, delta), MathOP::scale(basic, delta));
        delta = glm::vec3{-2.0f, -3.0f, -4.0f};
        MathOP::printMatrixs(glm::scale(basic, delta), MathOP::scale(basic, delta));
    }
    // Translate
    {
        std::cout << "\tTranslate" << std::endl;
        glm::vec3   delta{1.5f, 1.5f, 1.5f};
        MathOP::printMatrixs(glm::translate(basic, delta), MathOP::translate(basic, delta));
        delta = glm::vec3{0.5f, 0.5f, 0.5f};
        MathOP::printMatrixs(glm::translate(basic, delta), MathOP::translate(basic, delta));
        delta = glm::vec3{0.0f, 0.0f, 0.0f};
        MathOP::printMatrixs(glm::translate(basic, delta), MathOP::translate(basic, delta));
        delta = glm::vec3{2.0f, 3.0f, 4.0f};
        MathOP::printMatrixs(glm::translate(basic, delta), MathOP::translate(basic, delta));
        delta = glm::vec3{-2.0f, -3.0f, -4.0f};
        MathOP::printMatrixs(glm::translate(basic, delta), MathOP::translate(basic, delta));
    }
    // Rotate
    {
        std::cout << "\tRotate" << std::endl;
        glm::vec3   delta{1.5f, 1.5f, 1.5f};
        float       angle = glm::radians(30.0f);
        MathOP::printMatrixs(glm::rotate(basic, angle, delta), MathOP::rotate(basic, angle, delta));
        delta = glm::vec3{0.5f, 0.75f, 0.0f};
        angle = glm::radians(30.0f);
        MathOP::printMatrixs(glm::rotate(basic, angle, delta), MathOP::rotate(basic, angle, delta));
        delta = glm::vec3{1.0f, 0.0f, 0.0f};
        angle = glm::radians(30.0f);
        MathOP::printMatrixs(glm::rotate(basic, angle, delta), MathOP::rotate(basic, angle, delta));
        delta = glm::vec3{2.0f, 3.0f, 4.0f};
        angle = glm::radians(45.0f);
        MathOP::printMatrixs(glm::rotate(basic, angle, delta), MathOP::rotate(basic, angle, delta));
        delta = glm::vec3{-2.0f, -3.0f, -4.0f};
        angle = glm::radians(45.0f);
        MathOP::printMatrixs(glm::rotate(basic, angle, delta), MathOP::rotate(basic, angle, delta));
    }
    
    // LookAt
    {
        std::cout << "\tLookAt" << std::endl;
        glm::vec3   pos{0.0f, 0.0f, 0.0f};
        glm::vec3   front{1.0f, 1.0f, 0.0f};
        glm::vec3   up{0.0f, 0.0f, 1.0f};
        MathOP::printMatrixs(glm::lookAt(pos, pos + front, up), MathOP::lookAt(pos, pos + front, up));
        front = glm::vec3(-1.0f, -1.0f, -1.0f);
        up = glm::vec3(0.0f, 0.0f, 1.0f);
        MathOP::printMatrixs(glm::lookAt(pos, pos + front, up), MathOP::lookAt(pos, pos + front, up));
    }

    // Projection
    {
        std::cout << "\tOrtho Projection" << std::endl;
        MathOP::printMatrixs(glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f),
                            MathOP::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f));
        MathOP::printMatrixs(glm::ortho(-100.0f, 400.0f, -200.0f, 300.0f, 0.1f, 300.0f),
                            MathOP::ortho(-100.0f, 400.0f, -200.0f, 300.0f, 0.1f, 300.0f));
        
        std::cout << "\tPerspective Projection" << std::endl;
        MathOP::printMatrixs(glm::perspective(glm::radians(30.0f), 16.0f / 9.0f, 0.1f, 100.0f),
                            MathOP::perspective(glm::radians(30.0f), 16.0f / 9.0f, 0.1f, 100.0f));
        MathOP::printMatrixs(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 300.0f),
                            MathOP::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 300.0f));
    }
};

#endif