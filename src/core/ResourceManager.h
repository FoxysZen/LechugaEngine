#pragma once
#include <FileSystem.h>
#include <Font.h>
#include <Mesh.h>
#include <MTLParser.h>
#include <OBJParser.h>
#include <ShaderProgram.h>
#include <Texture.h>
#include <unordered_map>

class ResourceManager
{
    public:
        ResourceManager();
        ~ResourceManager();

        Mesh* loadMesh(std::string path);
        Texture* loadTexture(std::string path);
        ShaderProgram* loadShader(std::string vertPath, std::string fragPath);
        Font* loadFont(std::string fntPath, std::string texturePath);

    private:
        std::unordered_map<std::string, Mesh*> meshes;
        std::unordered_map<std::string, ShaderProgram*> shaders;
        std::unordered_map<std::string, Texture*> textures;
        std::unordered_map<std::string, Font*> fonts;
};