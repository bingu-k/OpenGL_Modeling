#ifndef COMMON_HPP
#define COMMON_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>
#include <memory>

std::string file_loader(const std::filesystem::path& filePath)
{
    std::ifstream       ifs(filePath);
    std::stringstream   ss;

    if (!ifs.is_open())
        throw std::string("Error: Failed to open file: ") + filePath.string();
    ss << ifs.rdbuf();
    ifs.close();
    return (ss.str());
};

#endif