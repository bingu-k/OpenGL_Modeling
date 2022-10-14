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

    void    CreatePlane(void);

    void    addTexture(const Texture& texture)
    { textures.push_back(texture); };
    void    addObject(const Object& object)
    { objects.push_back(object); };

    void    Rendering(void);

    const GLuint&   Get(void) const
    { return (this->id); };
    void    Use(void)
    { glUseProgram(this->id); };
    ~Program();
private:
    GLuint                  id { 0 };
    std::vector<Texture>    textures {};
    std::vector<Object>     objects {};

    Program() {};
    void    init(const std::filesystem::path& vertexShaderPath
                , const std::filesystem::path& fragmentShaderPath);
    void    checkError(void);
};

Program::~Program()
{
    textures.clear();
    objects.clear();
    glDeleteProgram(this->id);
};

std::unique_ptr<Program> Program::Create(const std::filesystem::path& vertexShaderPath
                                        , const std::filesystem::path& fragmentShaderPath)
{
    std::unique_ptr<Program>    program = std::unique_ptr<Program>(new Program());
    program->init(vertexShaderPath, fragmentShaderPath);
    return (std::move(program));
};

void    Program::CreatePlane(void)
{
    Object  object;

    object.CreatePlane();
    addObject(object);
};

void    Program::Rendering(void)
{
    static GLfloat  foo = 0.0f;
    
    Use();
    foo += 0.01f;
    glUniform1f(glGetUniformLocation(this->id, "foo"), sinf(foo));

    // 텍스처 바인딩
    for (unsigned int idx = 0; idx < textures.size(); ++idx)
    {
        glActiveTexture(GL_TEXTURE0 + idx);
        glBindTexture(GL_TEXTURE_2D, textures.at(idx).Get());
        glUniform1i(glGetUniformLocation(this->id, textures.at(idx).GetName().c_str()), 0);
    }
    // 그리기
    for (unsigned int idx = 0; idx < objects.size(); ++idx)
        objects.at(idx).Draw();
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