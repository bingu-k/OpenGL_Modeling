#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Common.hpp"

enum lightType{
    POINT, DIRECTION, SPOT
};

class Light
{
public:
    static std::unique_ptr<Light>    CreatePointLight(const glm::vec3& pos, const glm::vec3& ambient,
                                                    const glm::vec3& diffuse, const glm::vec3& specular);
    static std::unique_ptr<Light>    CreateDirectionLight(const glm::vec3& dir, const glm::vec3& ambient,
                                                        const glm::vec3& diffuse, const glm::vec3& specular);
    static std::unique_ptr<Light>    CreateSpotLight(const glm::vec3& pos, const glm::vec3& ambient,
                                                    const glm::vec3& diffuse, const glm::vec3& specular,
                                                    const glm::vec3& dir, const glm::vec2& cutoff);
    
    ~Light() {};

    void    setLightType(const unsigned int& type)
    { this->LightType = type; };
    void    setPos(const glm::vec3& pos)
    { this->pos = pos; };
    void    setDir(const glm::vec3& dir)
    { this->dir = dir; };
    void    setCutoff(const glm::vec2& cutoff)
    { this->cutoff = cutoff; };
    void    setLightComponent(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
    { this->ambient = ambient; this->diffuse = diffuse; this->specular = specular; };

    glm::vec3   GetPosition(void) const
    { return (this->pos); };
    
    void    setUniform(Program* program, int idx);
private:
    unsigned int    LightType;

    glm::vec3   pos{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3   ambient{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3   diffuse{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3   specular{ glm::vec3(0.0f, 0.0f, 0.0f) };

    glm::vec3   dir{ glm::vec3(0.0f, 0.0f, 0.0f) };

    glm::vec2   cutoff{ glm::vec2(0.0f, 0.0f) };

    Light() {};
    void    setPointUniform(Program* program, int idx);
    void    setDirectionUniform(Program* program);
    void    setSpotUniform(Program* program);
};

std::unique_ptr<Light>   Light::CreatePointLight(const glm::vec3& pos, const glm::vec3& ambient,
                                                const glm::vec3& diffuse, const glm::vec3& specular)
{
    std::unique_ptr<Light>  light = std::unique_ptr<Light>(new Light());
    light->setLightType(POINT);
    light->setPos(pos);
    light->setLightComponent(ambient, diffuse, specular);
    return (std::move(light));
};

std::unique_ptr<Light>   Light::CreateDirectionLight(const glm::vec3& dir, const glm::vec3& ambient,
                                                const glm::vec3& diffuse, const glm::vec3& specular)
{
    std::unique_ptr<Light>  light = std::unique_ptr<Light>(new Light());
    light->setLightType(DIRECTION);
    light->setDir(dir);
    light->setLightComponent(ambient, diffuse, specular);
    return (std::move(light));
};
std::unique_ptr<Light>   Light::CreateSpotLight(const glm::vec3& pos, const glm::vec3& ambient,
                                                const glm::vec3& diffuse, const glm::vec3& specular,
                                                const glm::vec3& dir, const glm::vec2& cutoff)
{
    std::unique_ptr<Light>  light = std::unique_ptr<Light>(new Light());
    light->setLightType(SPOT);
    light->setPos(pos);
    light->setDir(dir);
    light->setCutoff(cutoff);
    light->setLightComponent(ambient, diffuse, specular);
    return (std::move(light));
};


void    Light::setUniform(Program* program, int idx)
{
    if (this->LightType == POINT)
        this->setPointUniform(program, idx);
    else if (this->LightType == DIRECTION)
        this->setDirectionUniform(program);
    else if (this->LightType == SPOT)
        this->setSpotUniform(program);
};

void    Light::setPointUniform(Program* program, int idx)
{
    program->Use();
    // program->setUniform(true, "lightswitch[0]");
    program->setUniform(this->pos, "pointLight[" + std::to_string(idx) + "].position");
    program->setUniform(this->ambient, "pointLight[" + std::to_string(idx) + "].ambient");
    program->setUniform(this->diffuse, "pointLight[" + std::to_string(idx) + "].diffuse");
    program->setUniform(this->specular, "pointLight[" + std::to_string(idx) + "].specular");
};
void    Light::setDirectionUniform(Program* program)
{
    program->Use();
    // program->setUniform(true, "lightswitch[1]");
    program->setUniform(this->dir, "directionLight.direction");
    program->setUniform(this->ambient, "directionLight.ambient");
    program->setUniform(this->diffuse, "directionLight.diffuse");
    program->setUniform(this->specular, "directionLight.specular");
};
void    Light::setSpotUniform(Program* program)
{
    program->Use();
    // program->setUniform(true, "lightswitch[2]");
    program->setUniform(this->pos, "spotLight.position");
    program->setUniform(this->dir, "spotLight.direction");
    program->setUniform(this->cutoff, "spotLight.cutoff");
    program->setUniform(this->ambient, "spotLight.ambient");
    program->setUniform(this->diffuse, "spotLight.diffuse");
    program->setUniform(this->specular, "spotLight.specular");
};
#endif