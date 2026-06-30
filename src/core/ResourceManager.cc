#include <ResourceManager.h>

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager()
{
    for (auto &[path, mesh] : skinnedMeshes)
        delete mesh;
}

SkinnedMesh *ResourceManager::loadSkinnedMesh(const std::string &path)
{
    if (skinnedMeshes.count(path) > 0)
        return skinnedMeshes[path];

    SkinnedMeshData data = GLTFLoader::load(path, "assets/textures/");

    SkinnedMesh *mesh = new SkinnedMesh();
    for (int i = 0; i < (int)data.subMeshes.size(); ++i)
    {
        auto &sub = data.subMeshes[i];
        Texture *tex = nullptr;

        if (!sub.texturePath.empty())
        {
            tex = loadTexture(sub.texturePath);
        }
        else if (!sub.embeddedTexture.empty())
        {
            std::string key = path + "_embedded_" + std::to_string(i);

            if (textures.count(key) > 0)
            {
                tex = textures[key];
            }
            else
            {
                int w, h, channels;
                unsigned char *pixels = stbi_load_from_memory(
                    sub.embeddedTexture.data(),
                    (int)sub.embeddedTexture.size(),
                    &w, &h, &channels, 4);

                if (pixels)
                {
                    tex = new Texture();
                    tex->loadFromMemory(pixels, w, h);
                    stbi_image_free(pixels);
                    textures[key] = tex;
                }
                else
                {
                    Logger::error("ResourceManager: failed to decode embedded texture");
                }
            }
        }

        mesh->addSubMesh(sub.vertices, sub.indices, tex);
    }

    mesh->calculateBounds();
    mesh->setSkeleton(data.skeleton);

    skinnedMeshes[path] = mesh;
    return mesh;
}

Texture *ResourceManager::loadTexture(std::string path)
{
    if (textures.count(path) > 0)
    {
        return textures[path];
    }
    
    Texture *texture = new Texture();
    texture->load(path);
    textures[path] = texture;
    return texture;
}

ShaderProgram *ResourceManager::loadShader(std::string vertPath, 
                                           std::string fragPath)
{
    ShaderProgram *shader = new ShaderProgram();
    shader->load(vertPath, fragPath);
    shaders[vertPath] = shader;
    return shader;
}

Font *ResourceManager::loadFont(std::string fntPath, std::string texturePath)
{
    if (fonts.count(fntPath) > 0)
        return fonts[fntPath];
    
    Font *font = new Font();
    font->load(fntPath, loadTexture(texturePath));
    fonts[fntPath] = font;
    return font;
}
