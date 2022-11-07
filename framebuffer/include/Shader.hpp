#ifndef SHADER_HPP
#define SHADER_HPP

#include "Common.hpp"


class Shader
{
public:
    static std::unique_ptr<Shader>   Create(const std::filesystem::path& filePath, GLenum shaderType);

    const GLuint&  Get(void) const
    { return (this->id); };
    ~Shader()
    { glDeleteShader(this->id); };
private:
    GLuint  id { 0 };

    Shader() {};
    void    init(const std::filesystem::path& filePath, GLenum shaderType);
    void    checkError(void);
};

std::unique_ptr<Shader> Shader::Create(const std::filesystem::path& filePath, GLenum shaderType)
{
    std::unique_ptr<Shader> shader = std::unique_ptr<Shader>(new Shader());
    shader->init(filePath, shaderType);
    return (std::move(shader));
};

void    Shader::init(const std::filesystem::path& filePath, GLenum shaderType)
{
    std::string source = file_loader(filePath);
    const char* source_ptr = source.data();

    this->id = glCreateShader(shaderType); // 셰이더 오브젝트 생성
    glShaderSource(this->id, 1, &source_ptr, nullptr); // 셰이더 소스파일 로드
    glCompileShader(this->id); // 셰이더 컴파일
    checkError();
};

void    Shader::checkError(void)
{
    int success;

    glGetShaderiv(this->id, GL_COMPILE_STATUS, &success); // 잘 됐는지 확인
    if (!success)
    {
        char    log[512];
        glGetShaderInfoLog(this->id, 512, nullptr, log); // 로그 확인
        throw std::string("Error: Failed to compile shader:\n") + log;
    }
};

#endif