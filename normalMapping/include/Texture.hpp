#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Common.hpp"
#include "Program.hpp"

class Texture
{
public:
    static std::unique_ptr<Texture>    Load(const std::filesystem::path& filePath,
                                    		std::string name);

    const GLuint&       Get(void) const
    { return (this->id); };
    const std::string&  GetName(void) const
    { return (this->name); };
	void	bind(unsigned int idx);
private:
    GLuint      id { 0 };
    std::string name { "" };

    Texture() {};
    void    init(void);
    void    LoadFile(const std::filesystem::path& filePath, std::string name);
    void    SetWrap(GLuint wrapS, GLuint wrapT);
    void    SetFliter(GLuint filterMin, GLuint filterMag);
};

std::unique_ptr<Texture>    Texture::Load(const std::filesystem::path& filePath, std::string name)
{
    std::unique_ptr<Texture>	texture = std::unique_ptr<Texture>(new Texture());
    texture->init();
    texture->LoadFile(filePath, name);
	return (std::move(texture));
};

void	Texture::bind(unsigned int idx)
{
	glActiveTexture(GL_TEXTURE0 + idx);
	glBindTexture(GL_TEXTURE_2D, this->id);
};

void    Texture::init()
{
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
    SetWrap(GL_REPEAT, GL_REPEAT);
    SetFliter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
};

void    Texture::LoadFile(const std::filesystem::path& filePath, std::string name)
{
    this->name = name;
    int width, height, channel;
    unsigned char*  data = stbi_load(filePath.string().c_str(), &width, &height, &channel, 4);
    if (data == nullptr)
        throw std::string("Error: Failed to open image: ") + filePath.string();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
};

void    Texture::SetWrap(GLuint wrapS, GLuint wrapT)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
};

void    Texture::SetFliter(GLuint filterMin, GLuint filterMag)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
};

#endif