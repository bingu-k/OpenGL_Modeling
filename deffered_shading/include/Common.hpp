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

const float width(1280.0f), height(960.0f);

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


#endif