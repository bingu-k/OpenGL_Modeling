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
                                                    const glm::vec3& diffuse, const glm::vec3& specular,
                                                    const float& distance);
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
    void    setDistance(const float& distance)
    { this->distance = distance; };
    void    setDir(const glm::vec3& dir)
    { this->dir = dir; this->directional = true; };
    void    setCutoff(const glm::vec2& cutoff)
    { this->cutoff = cutoff; };
    void    setLightComponent(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
    { this->ambient = ambient; this->diffuse = diffuse; this->specular = specular; };
    
    void    setUniform(Program* program);
private:
    unsigned int    LightType;

    glm::vec3   pos{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3   ambient{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3   diffuse{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3   specular{ glm::vec3(0.0f, 0.0f, 0.0f) };

    bool        directional{ false };
    glm::vec3   dir{ glm::vec3(0.0f, 0.0f, 0.0f) };

    float       distance {0.0f};
    glm::vec2   cutoff{ glm::vec2(0.0f, 0.0f) };

    Light() {};
    void    setPointUniform(Program* program);
    void    setDirectionUniform(Program* program);
    void    setSpotUniform(Program* program);
};

std::unique_ptr<Light>   Light::CreatePointLight(const glm::vec3& pos, const glm::vec3& ambient,
                                                const glm::vec3& diffuse, const glm::vec3& specular,
                                                const float& distance)
{
    std::unique_ptr<Light>  light = std::unique_ptr<Light>(new Light());
    light->setLightType(POINT);
    light->setPos(pos);
    light->setDistance(distance);
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


void    Light::setUniform(Program* program)
{
    if (this->LightType == POINT)
        this->setPointUniform(program);
    else if (this->LightType == DIRECTION)
        this->setPointUniform(program);
    else if (this->LightType == SPOT)
        this->setPointUniform(program);
};

void    Light::setPointUniform(Program* program)
{
    program->setUniform(this->pos, "PointLight.position");
    program->setUniform(this->ambient, "PointLight.ambient");
    program->setUniform(this->diffuse, "PointLight.diffuse");
    program->setUniform(this->specular, "PointLight.specular");
};
void    Light::setDirectionUniform(Program* program)
{
    program->setUniform(this->dir, "DirectionLight.direction");
    program->setUniform(this->ambient, "DirectionLight.ambient");
    program->setUniform(this->diffuse, "DirectionLight.diffuse");
    program->setUniform(this->specular, "DirectionLight.specular");
};
void    Light::setSpotUniform(Program* program)
{
    program->setUniform(this->pos, "SpotLight.position");
    program->setUniform(this->dir, "SpotLight.direction");
    program->setUniform(this->cutoff, "SpotLight.cutoff");
    program->setUniform(this->ambient, "SpotLight.ambient");
    program->setUniform(this->diffuse, "SpotLight.diffuse");
    program->setUniform(this->specular, "SpotLight.specular");
};
#endif