#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "Common.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Object.hpp"

class Program
{
public:
    static std::unique_ptr<Program> Create(const std::filesystem::path& vertexShaderPath
                                        , const std::filesystem::path& fragmentShaderPath);

    void    Rendering(void);

    const GLuint&   Get(void) const
    { return (this->id); };
    void    Use(void)
    { glUseProgram(this->id); };
    ~Program();
private:
    GLuint                  id { 0 };

    Program() {};
    void    init(const std::filesystem::path& vertexShaderPath
                , const std::filesystem::path& fragmentShaderPath);
    void    checkError(void);
};

Program::~Program()
{ glDeleteProgram(this->id); };

std::unique_ptr<Program> Program::Create(const std::filesystem::path& vertexShaderPath
                                        , const std::filesystem::path& fragmentShaderPath)
{
    std::unique_ptr<Program>    program = std::unique_ptr<Program>(new Program());
    program->init(vertexShaderPath, fragmentShaderPath);
    return (std::move(program));
};

void    Program::Rendering(void)
{
    static GLfloat  foo = 0.0f;
    
    Use();
    foo += 0.01f;
    glUniform1f(glGetUniformLocation(this->id, "foo"), sinf(foo));
};

void    Program::init(const std::filesystem::path& vertexShaderPath
                    , const std::filesystem::path& fragmentShaderPath)
{
    std::unique_ptr<Shader> vertexShader = Shader::Create(vertexShaderPath, GL_VERTEX_SHADER);
    std::unique_ptr<Shader> fragmentShader = Shader::Create(fragmentShaderPath, GL_FRAGMENT_SHADER);
    
    // 셰이더 프로그램 링크
    this->id = glCreateProgram();
    glAttachShader(this->id, vertexShader->Get());
    glAttachShader(this->id, fragmentShader->Get());
    glLinkProgram(this->id);
    
    checkError();
};

void    Program::checkError(void)
{
    int success;

    glGetProgramiv(this->id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(this->id, 512, nullptr, log);
        throw std::string("Error: Failed to link shader program:\n") + log;
    }
};

#endif