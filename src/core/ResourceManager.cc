#include <ResourceManager.h>

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager() {}

Mesh* ResourceManager::loadMesh(std::string path)
{
    if (meshes.count(path) > 0)
        return meshes[path];

    std::string mtlPath = path.substr(0, path.find_last_of('.')) + ".mtl";
    
    auto groups = OBJParser::getVertices(path);
    auto materials = MTLParser::getTexture(mtlPath);
    
    Mesh* mesh = new Mesh();
    for (auto& [materialName, vertices] : groups)
    {
        std::string texPath = materials[materialName];
        Texture* texture = loadTexture("assets/textures/" + texPath);
        mesh->addSubMesh(vertices, texture);
    }
    mesh->calculateBounds();
    
    meshes[path] = mesh;
    return mesh;
}

Texture* ResourceManager::loadTexture(std::string path)
{
    if (textures.count(path) > 0)
    {
        return textures[path];
    }
    
    Texture* texture = new Texture();
    texture->load(path);
    textures[path] = texture;
    return texture;
}

ShaderProgram* ResourceManager::loadShader(std::string vertPath, 
                                           std::string fragPath)
{
    ShaderProgram* shader = new ShaderProgram();
    shader->load(vertPath, fragPath);
    shaders[vertPath] = shader;
    return shader;
}
