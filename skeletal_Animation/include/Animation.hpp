#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Common.hpp"
#include "Bone.hpp"
#include "Model.hpp"
#include <functional>

struct AssimpNodeData
{
    glm::mat4   transformation;
    std::string name;
    int         childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;
    Animation(const std::string& animationPath, Model* model);
    ~Animation() {};

    Bone*   FindBone(const std::string& name);

    inline float    GetDuration(void) const { return (this->duration); };
    inline float    GetTicksPerSecond(void) const { return (this->ticksPerSecond); };
    inline const AssimpNodeData& GetRootNode(void) const { return (this->rootNode); };
    inline const std::map<std::string, BoneInfo>&   GetBoneIDMap(void)
    { return (this->boneInfoMap); };
private:
    float   duration;
    int     ticksPerSecond;
    std::vector<Bone>   bones;
    AssimpNodeData      rootNode;
    std::map<std::string, BoneInfo> boneInfoMap;

    void    ReadMissingBones(const aiAnimation* animation, Model& model);
    void    ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};

Animation::Animation(const std::string& animationPath, Model* model)
{
    Assimp::Importer    importer;
    const aiScene*      scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);

    auto    animation = scene->mAnimations[0];
    this->duration = animation->mDuration;
    this->ticksPerSecond = animation->mTicksPerSecond;
    // aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
    // globalTransformation = globalTransformation.Inverse();
    ReadHeirarchyData(this->rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
};

Bone*   Animation::FindBone(const std::string& name)
{
    auto    iter = std::find_if(this->bones.begin(), this->bones.end(),
                                [&](const Bone& bone)
                                { return bone.GetBoneName() == name; } );
    if (iter == this->bones.end())
        return (nullptr);
    else
        return &(*iter);
};

void    Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
    int     size = animation->mNumChannels;
    auto&   tmpBoneInfoMap = model.GetBoneInfoMap();
    int&    tmpBoneCount = model.GetBoneCount();

    for (int i = 0; i < size; ++i)
    {
        auto        channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        if (tmpBoneInfoMap.find(boneName) == tmpBoneInfoMap.end())
        {
            tmpBoneInfoMap[boneName].id = tmpBoneCount;
            ++tmpBoneCount;
        }
        this->bones.push_back(Bone(channel->mNodeName.data,
                                    tmpBoneInfoMap[channel->mNodeName.data].id, channel));
    }
    this->boneInfoMap = tmpBoneInfoMap;
};

void    Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;
    for (int i = 0; i < src->mNumChildren; ++i)
    {
        AssimpNodeData  newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
};

#endif