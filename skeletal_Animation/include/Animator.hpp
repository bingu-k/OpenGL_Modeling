#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include "Common.hpp"
#include "Animation.hpp"
#include "Bone.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Animator
{
public:
    Animator() = default;
    Animator(Animation* animation);
    ~Animator() = default;

    void    UpdateAnimation(float dt);
    void    PlayAnimation(Animation* pAnimation);
    void    CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    std::vector<glm::mat4>  GetFinalBoneMatrices(void) const
    { return (this->finalBoneMatrices); };
private:
    std::vector<glm::mat4>  finalBoneMatrices;
    Animation*              currentAnimation {nullptr};
    float                   currentTime {0.0f};
    float                   deltaTime {0.0f};
};

Animator::Animator(Animation* animation)
{
    this->currentTime = 0.0f;
    this->currentAnimation = animation;
    this->finalBoneMatrices.reserve(100);
    for (int i = 0; i < 100; ++i)
        this->finalBoneMatrices.push_back(glm::mat4(1.0f));
};

void    Animator::UpdateAnimation(float dt)
{
    this->deltaTime = dt;
    if (this->currentAnimation)
    {
        this->currentTime += this->currentAnimation->GetTicksPerSecond() * dt;
        this->currentTime = fmod(this->currentTime, this->currentAnimation->GetDuration());
        CalculateBoneTransform(&this->currentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
};

void    Animator::PlayAnimation(Animation* pAnimation)
{
    this->currentAnimation = pAnimation;
    this->currentTime = 0.0f;
};

void    Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4   nodeTransform = node->transformation;
    Bone*       bone = this->currentAnimation->FindBone(nodeName);
    if (bone)
    {
        bone->Update(this->currentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4   globalTransformation = parentTransform * nodeTransform;
    auto        boneInfoMap = this->currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4   offset = boneInfoMap[nodeName].offset;
        this->finalBoneMatrices[index] = globalTransformation * offset;
    }
    for (int i = 0; i < node->childrenCount; ++i)
        CalculateBoneTransform(&node->children[i], globalTransformation);
};


#endif