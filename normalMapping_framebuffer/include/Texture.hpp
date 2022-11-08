#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Common.hpp"
#include "Program.hpp"

class Texture
{
public:
    static std::unique_ptr<Texture> Create(const float& width, const float& height,
                                        const unsigned int& format, GLenum TextureTarget = GL_TEXTURE_2D);
    static std::unique_ptr<Texture> Load(const std::filesystem::path& filePath,
                                    	std::string name, GLenum TextureTarget = GL_TEXTURE_2D);

    const GLuint&       Get(void) const
    { return (this->id); };
    const std::string&  GetName(void) const
    { return (this->name); };
    const GLfloat&      GetWidth(void) const
    { return (this->width); };
    const GLfloat&      GetHeight(void) const
    { return (this->height); };

	void	bind(unsigned int idx);
private:
    GLuint      id { 0 };
    std::string name { "" };
    GLenum      target;
    GLfloat     width, height;
    GLuint      format;

    Texture() {};
    void    init(GLenum TextureTarget);
    void    LoadFile(const std::filesystem::path& filePath, std::string name);
    void    SetWrap(GLuint wrapS, GLuint wrapT);
    void    SetFliter(GLuint filterMin, GLuint filterMag);
    void    SetTexImage(const float& width, const float& height,
                        const GLuint& format, unsigned char* data = nullptr);
};

std::unique_ptr<Texture>    Texture::Create(const float& width, const float& height,
                                            const unsigned int& format, GLenum TextureTarget)
{
    std::unique_ptr<Texture>	texture = std::unique_ptr<Texture>(new Texture());
    texture->init(TextureTarget);
    texture->SetTexImage(width, height, format, nullptr);
    texture->SetFliter(GL_LINEAR, GL_LINEAR);
	return (std::move(texture));
}

std::unique_ptr<Texture>    Texture::Load(const std::filesystem::path& filePath, std::string name,
                                        GLenum TextureTarget)
{
    std::unique_ptr<Texture>	texture = std::unique_ptr<Texture>(new Texture());
    texture->init(TextureTarget);
    texture->SetWrap(GL_REPEAT, GL_REPEAT);
    texture->SetFliter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    texture->LoadFile(filePath, name);
	return (std::move(texture));
};

void	Texture::bind(unsigned int idx)
{
	glActiveTexture(GL_TEXTURE0 + idx);
	glBindTexture(this->target, this->id);
};

void    Texture::init(GLenum TextureTarget)
{
    this->target = TextureTarget;
	glGenTextures(1, &this->id);
	glBindTexture(this->target, this->id);
};

void    Texture::LoadFile(const std::filesystem::path& filePath, std::string name)
{
    stbi_set_flip_vertically_on_load(true);
    this->name = name;
    int width, height, channel;
    unsigned char*  data = stbi_load(filePath.string().c_str(), &width, &height, &channel, 4);
    if (data == nullptr)
        throw std::string("Error: Failed to open image: ") + filePath.string();

    SetTexImage(static_cast<float>(width), static_cast<float>(height), GL_RGBA, data);
    glGenerateMipmap(this->target);
    stbi_image_free(data);
};

void    Texture::SetWrap(GLuint wrapS, GLuint wrapT)
{
    glTexParameteri(this->target, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(this->target, GL_TEXTURE_WRAP_T, wrapT);
};

void    Texture::SetFliter(GLuint filterMin, GLuint filterMag)
{
    glTexParameteri(this->target, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(this->target, GL_TEXTURE_MAG_FILTER, filterMag);
};


void    Texture::SetTexImage(const float& width, const float& height,
                            const GLuint& format, unsigned char* data)
{
    this->width = width;
    this->height = height;
    this->format = format;
    glTexImage2D(this->target, 0, this->format, this->width, this->height, 0,
                this->format, GL_UNSIGNED_BYTE, data);
};

#endif