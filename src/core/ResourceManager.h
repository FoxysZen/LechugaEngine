#pragma once
#include <Animation.h>
#include <FileSystem.h>
#include <Font.h>
#include <GLTFLoader.h>
#include <ShaderProgram.h>
#include <SkinnedMesh.h>
#include <Texture.h>
#include <unordered_map>

class ResourceManager
{
    public:
        ResourceManager();
        ~ResourceManager();

        SkinnedMesh *loadSkinnedMesh(const std::string &path);
        Texture *loadTexture(std::string path);
        ShaderProgram *loadShader(std::string vertPath, std::string fragPath);
        Font *loadFont(std::string fntPath, std::string texturePath);

    private:
        std::unordered_map<std::string, SkinnedMesh*> skinnedMeshes;
        std::unordered_map<std::string, ShaderProgram*> shaders;
        std::unordered_map<std::string, Texture*> textures;
        std::unordered_map<std::string, Font*> fonts;
};