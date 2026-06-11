#include <SceneLoader.h>

SceneLoader::SceneLoader(Scene *_scene, ResourceManager *_resourceManager)
{
    scene = _scene;
    resourceManager = _resourceManager;
}

SceneLoader::~SceneLoader()
{

}

void SceneLoader::loadScene(std::string sceneName)
{
    std::string file = FileSystem::readFile("assets/scenes/" + sceneName);
    nlohmann::json json = nlohmann::json::parse(file);

    int size = json["entities"].size();
    for (int i = 0; i < size; ++i)
    {
        EntityID id = scene->createEntity();

        glm::vec3 position = glm::vec3(
            json["entities"][i]["transform"]["position"][0],
            json["entities"][i]["transform"]["position"][1],
            json["entities"][i]["transform"]["position"][2]
        );

        glm::vec3 rotation = glm::vec3(
            json["entities"][i]["transform"]["rotation"][0],
            json["entities"][i]["transform"]["rotation"][1],
            json["entities"][i]["transform"]["rotation"][2]
        );

        glm::vec3 scale = glm::vec3(
            json["entities"][i]["transform"]["scale"][0],
            json["entities"][i]["transform"]["scale"][1],
            json["entities"][i]["transform"]["scale"][2]
        );

        scene->addTransform(id, {position, rotation, scale});

        int nLods = json["entities"][i]["mesh"]["lods"].size();
        std::vector<LOD> lods(nLods);
        for (int j = 0; j < nLods; ++j)
        {
            lods[j].mesh = resourceManager->loadMesh(
                json["entities"][i]["mesh"]["lods"][j]["path"]);
            lods[j].maxDistance = 
                json["entities"][i]["mesh"]["lods"][j]["maxDistance"];
        }

        ShaderProgram *shad = resourceManager->loadShader(
            json["entities"][i]["mesh"]["shader"][0], 
            json["entities"][i]["mesh"]["shader"][1]);
        
        int nTextures = json["entities"][i]["mesh"]["textures"].size();
        std::vector<Texture*> textures(nTextures);
        for (int i = 0; i < nTextures; ++i)
        {
            textures[i] = resourceManager->loadTexture(
                json["entities"][i]["mesh"]["textures"][i]);
        }

        scene->addMesh(id, {lods, shad, textures});
    }

    size = json["lights"].size();
    for (int i = 0; i < size; ++i)
    {
        EntityID id = scene->createEntity();

        glm::vec3 position = {
            json["lights"][i]["position"][0],
            json["lights"][i]["position"][1],
            json["lights"][i]["position"][2]
        };

        glm::vec3 color = {
            json["lights"][i]["color"][0],
            json["lights"][i]["color"][1],
            json["lights"][i]["color"][2]
        };

        scene->addLight(id, {position, color, json["lights"][i]["intensity"]});
    }
}
