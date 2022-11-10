#ifndef BONE_HPP
#define BONE_HPP

#include "Common.hpp"
#include <assimp/scene.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "AssimpGLMHelpers.hpp"

struct KeyPosition
{
    glm::vec3   position;
    float       timeStamp;
};

struct KeyRotation
{
    glm::quat   orientation;
    float       timeStamp;
};

struct KeyScale
{
    glm::vec3   scale;
    float       timeStamp;
};

class Bone
{
public:
    Bone() = default;
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);
    ~Bone() {};

    void    Update(float animation);

    glm::mat4   GetLocalTransform(void) { return (this->localTransform); };
    std::string GetBoneName(void) const { return (this->name); };
    int         GetBoneID(void) {return (this->ID); };

    int GetPositionIndex(float animationTime);
	int GetRotationIndex(float animationTime);
	int GetScaleIndex(float animationTime);
    
private:
    std::vector<KeyPosition>    position;
    std::vector<KeyRotation>    rotation;
    std::vector<KeyScale>       scale;
    int numPositions, numRotations, numScalings;

    glm::mat4   localTransform;
    std::string name;
    int         ID;

    float   GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;

    glm::mat4   InterpolatePosition(float animationTime);
    glm::mat4   InterpolateRotation(float animationTime);
    glm::mat4   InterpolateScaling(float animationTime);
};

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
: name(name), ID(ID), localTransform(1.0f)
{
    this->numPositions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < this->numPositions; ++positionIndex)
    {
        aiVector3D  aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float       timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
        data.timeStamp = timeStamp;
        this->position.push_back(data);
    }

    this->numRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < this->numRotations; ++rotationIndex)
    {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
        data.timeStamp = timeStamp;
        this->rotation.push_back(data);
    }

    this->numScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < this->numScalings; ++keyIndex)
    {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.scale = AssimpGLMHelpers::GetGLMVec(scale);
        data.timeStamp = timeStamp;
        this->scale.push_back(data);
    }
};

void    Bone::Update(float animationTime)
{
    glm::mat4   translation = InterpolatePosition(animationTime);
    glm::mat4   rotation = InterpolateRotation(animationTime);
    glm::mat4   scale = InterpolateScaling(animationTime);
    this->localTransform = translation * rotation * scale;
};

int Bone::GetPositionIndex(float animationTime)
{
    for (int index = 0; index < this->numPositions - 1; ++index)
    {
        if (animationTime < this->position[index + 1].timeStamp)
            return (index);
    }
    assert(0);
}

int Bone::GetRotationIndex(float animationTime)
{
    for (int index = 0; index < numRotations - 1; ++index)
    {
        if (animationTime < rotation[index + 1].timeStamp)
            return (index);
    }
    assert(0);
}

int Bone::GetScaleIndex(float animationTime)
{
    for (int index = 0; index < numScalings - 1; ++index)
    {
        if (animationTime < scale[index + 1].timeStamp)
            return (index);
    }
    assert(0);
}

float   Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
    float   scaleFactor = 0.0f;
    float   midWayLength = animationTime - lastTimeStamp;
    float   framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return (scaleFactor);
};

glm::mat4   Bone::InterpolatePosition(float animationTime)
{
    if (this->numPositions == 1)
        return (glm::translate(glm::mat4(1.0f), this->position[0].position));

	int     p0Index = GetPositionIndex(animationTime);
    int     p1Index = p0Index + 1;
    float   scaleFactor = GetScaleFactor(this->position[p0Index].timeStamp,
                                        this->position[p1Index].timeStamp,
                                        animationTime);
    glm::vec3   finalPosition = glm::mix(this->position[p0Index].position,
                                        this->position[p1Index].position,
                                        scaleFactor);
    return (glm::translate(glm::mat4(1.0f), finalPosition));
};

glm::mat4   Bone::InterpolateRotation(float animationTime)
{
    if (this->numRotations == 1)
    {
        auto    rotation = glm::normalize(this->rotation[0].orientation);
        return (glm::toMat4(rotation));
    }

    int     p0Index = GetRotationIndex(animationTime);
    int     p1Index = p0Index + 1;
    float   scaleFactor = GetScaleFactor(this->rotation[p0Index].timeStamp,
                                        this->rotation[p1Index].timeStamp,
                                        animationTime);
    glm::quat   finalRotation = glm::slerp(this->rotation[p0Index].orientation,
                                        this->rotation[p1Index].orientation,
                                        scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return (glm::toMat4(finalRotation));
};

glm::mat4   Bone::InterpolateScaling(float animationTime)
{
    if (this->numScalings == 1)
        return (glm::scale(glm::mat4(1.0f), this->scale[0].scale));

    int     p0Index = GetScaleIndex(animationTime);
    int     p1Index = p0Index + 1;
    float   scaleFactor = GetScaleFactor(this->scale[p0Index].timeStamp,
                                        this->scale[p1Index].timeStamp,
                                        animationTime);
    glm::vec3   finalScale = glm::mix(this->scale[p0Index].scale,
                                    this->scale[p1Index].scale,
                                    scaleFactor);
    return (glm::scale(glm::mat4(1.0f), finalScale));
};



#endif