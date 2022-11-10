#ifndef MODEL_HPP
#define MODEL_HPP

#include "Common.hpp"
#include "Program.hpp"
#include "Mesh.hpp"
#include "AssimpGLMHelpers.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int    TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

struct  BoneInfo
{
    int         id;
    glm::mat4   offset;
};

class Model
{
public:
    static std::unique_ptr<Model>   LoadModel(const std::string& path);

    ~Model() {};
    void    draw(Program* program);

    auto&   GetBoneInfoMap(void) { return (this->boneInfoMap); };
    int&    GetBoneCount(void) { return (this->boneCount); };

private:
    std::vector<mTexture>   textures_loaded;
    std::vector<Mesh>   meshes;
    std::string         directory;

    std::map<std::string, BoneInfo> boneInfoMap;
    int                             boneCount{ 0 };

    Model() {};
    void    init(const std::string& path);
    void    processNode(aiNode* node, const aiScene* scene);
    Mesh    processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<mTexture>   loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    void    SetVertexBoneDataDefault(mVertex& vertex);
    void    SetVertexBoneData(mVertex& vertex, int boneID, float weight);
    void    ExtractBoneWeightForVertices(std::vector<mVertex>& vertices, aiMesh* mesh, const aiScene* scene);
};

std::unique_ptr<Model>   Model::LoadModel(const std::string& path)
{
    std::unique_ptr<Model>  model = std::unique_ptr<Model>(new Model());
    model->init(path);
    return (std::move(model));
};

void    Model::draw(Program* program)
{
    for (auto& mesh : this->meshes)
        mesh.Draw(program);
};

void    Model::init(const std::string& path)
{
    Assimp::Importer    import;
    const aiScene*  scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw (import.GetErrorString());
    this->directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
};

void    Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
};

Mesh    Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<mVertex>    vertices;
    std::vector<GLuint>     indices;
    std::vector<mTexture>   textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        mVertex vertex;

        this->SetVertexBoneDataDefault(vertex);

        vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
        vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

        if(mesh->mTextureCoords[0])
        {
            glm::vec2   vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace  face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<mTexture>   diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                            "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<mTexture>   specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                            "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<mTexture>   normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                            "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	std::vector<mTexture>   heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                            "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    ExtractBoneWeightForVertices(vertices, mesh, scene);
    return (Mesh(vertices, indices, textures));
};

std::vector<mTexture>   Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<mTexture>   textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString    str;
        mat->GetTexture(type, i, &str);
        bool    skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); ++j)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break ;
            }
        }
        if (!skip)
        {
            mTexture    texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return (textures);
};

unsigned int    TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int    textureID;
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    unsigned char   *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return (textureID);
};

void    Model::SetVertexBoneDataDefault(mVertex& vertex)
{
    for (unsigned int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }
};

void    Model::SetVertexBoneData(mVertex& vertex, int boneID, float weight)
{
    for (unsigned int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.boneIDs[i] < 0)
        {
            vertex.boneIDs[i] = boneID;
            vertex.weights[i] = weight;
            break;
        }
    }
};

void    Model::ExtractBoneWeightForVertices(std::vector<mVertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (this->boneInfoMap.find(boneName) == this->boneInfoMap.end())
        {
            BoneInfo    newBoneInfo;
            newBoneInfo.id = this->boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            this->boneInfoMap[boneName] = newBoneInfo;
            boneID = this->boneCount;
            ++this->boneCount;
        }
        else
            boneID = this->boneInfoMap[boneName].id;
        assert(boneID != -1);
        auto    weights = mesh->mBones[boneIndex]->mWeights;
        int     numWeights = mesh->mBones[boneIndex]->mNumWeights;
        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int     vertexID = weights[weightIndex].mVertexId;
            float   weight = weights[weightIndex].mWeight;
            assert(vertexID <= vertices.size());
            SetVertexBoneData(vertices[vertexID], boneID, weight);
        }
    }
};

#endif