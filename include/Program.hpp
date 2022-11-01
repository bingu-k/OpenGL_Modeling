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

    void    setUniform(const int& value, const std::string& name)
    { glUniform1i(glGetUniformLocation(this->id, name.c_str()), value); };
    void    setUniform(const float& value, const std::string& name)
    { glUniform1f(glGetUniformLocation(this->id, name.c_str()), value); };
    void    setUniform(const glm::vec2& value, const std::string& name)
    { glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value)); };
    void    setUniform(const glm::vec3& value, const std::string& name)
    { glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value)); };
    void    setUniform(const glm::vec4& value, const std::string& name)
    { glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value)); };
    void    setUniform(const glm::mat4& value, const std::string& name)
    { glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); };
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
    
    this->Use();
    foo += 0.01f;
    this->setUniform(sinf(foo), "foo");
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