#ifndef COMMON_HPP
#define COMMON_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>
#include <memory>

const float width(800.0f), height(600.0f);

glm::vec3   pos(0.0f, 0.0f, 3.0f), front(0.0f, 0.0f, -1.0f), worldUp(0.0f, 1.0f, 0.0f), up(worldUp);
float       prevXPos(400.0f), prevYPos(300.0f), yaw(-90.0f), pitch(0.0f);

std::string file_loader(const std::filesystem::path& filePath)
{
    std::ifstream       ifs(filePath.c_str());
    std::stringstream   ss;

    if (!ifs.is_open())
        throw std::string("Error: Failed to open file: ") + filePath.string();
    ss << ifs.rdbuf();
    ifs.close();
    return (ss.str());
};

void    key_manager(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float Vel = 0.025;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pos += Vel * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pos -= Vel * front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pos += Vel * glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pos -= Vel * glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        pos += Vel * up;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        pos -= Vel * up;
};

void    updateCamera(void)
{
    front = glm::normalize(glm::vec3{cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                                    sin(glm::radians(pitch)),
                                    sin(glm::radians(yaw)) * cos(glm::radians(pitch))});
    up = glm::normalize(glm::cross(glm::normalize(glm::cross(front, worldUp)), front));
};
#endif