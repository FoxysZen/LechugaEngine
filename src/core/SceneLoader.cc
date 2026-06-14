#include <SceneLoader.h>

SceneLoader::SceneLoader(Scene *_scene, ResourceManager *_resourceManager,
    UIManager *_uiManager)
{
    scene = _scene;
    resourceManager = _resourceManager;
    uiManager = _uiManager;
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
        for (int j = 0; j < nTextures; ++j)
        {
            textures[j] = resourceManager->loadTexture(
                json["entities"][j]["mesh"]["textures"][j]);
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

    size = json["particles"].size();
    for (int i = 0; i < size; ++i)
    {
        glm::vec3 position = {
            json["particles"][i]["position"][0],
            json["particles"][i]["position"][1],
            json["particles"][i]["position"][2]
        };

        glm::vec3 direction = {
            json["particles"][i]["direction"][0],
            json["particles"][i]["direction"][1],
            json["particles"][i]["direction"][2]
        };

        glm::vec3 startColor = {
            json["particles"][i]["startColor"][0],
            json["particles"][i]["startColor"][1],
            json["particles"][i]["startColor"][2]
        };

        glm::vec3 endColor = {
            json["particles"][i]["endColor"][0],
            json["particles"][i]["endColor"][1],
            json["particles"][i]["endColor"][2]
        };

        float vel = json["particles"][i]["velocity"];
        float life = json["particles"][i]["lifeTime"];
        float startSize = json["particles"][i]["startSize"];
        float endSize = json["particles"][i]["endSize"];
        float sizeCurve = json["particles"][i]["sizeCurve"];
        float colorCurve = json["particles"][i]["colorCurve"];
        float spread = json["particles"][i]["spread"];
        float rate = json["particles"][i]["emissionRate"];
        float spiral = json["particles"][i]["spiralSpeed"];
        float gravity = json["particles"][i]["gravity"];
        int max = json["particles"][i]["maxParticles"];
        std::string type = json["particles"][i]["type"];
        std::string shape = json["particles"][i]["shape"];
        std::string mode = json["particles"][i]["renderMode"];
        std::string texture = json["particles"][i]["texture"];

        ShaderProgram* particleShader = resourceManager->loadShader(
            "assets/shaders/particleBillboard.vert",
            "assets/shaders/particleBillboard.frag"
        );

        ParticleType pType = (type == "BILLBOARD") ? 
            ParticleType::BILLBOARD : ParticleType::MESH;

        ParticleRenderMode pMode = (mode == "TEXTURE") ? 
            ParticleRenderMode::TEXTURE : ParticleRenderMode::COLOR;

        EmitterShape pShape;
        if (shape == "FOUNTAIN")
            pShape = EmitterShape::FOUNTAIN;
        else if (shape == "CIRCLE")
            pShape = EmitterShape::CIRCLE;
        else if (shape == "SPIRAL")
            pShape = EmitterShape::SPIRAL;
        else
            pShape = EmitterShape::POINT;

        ParticleSystem* ps = new ParticleSystem(particleShader, pType, pMode, 
            pShape, max);
        ps->setPosition(position);
        ps->setDirection(direction);
        ps->setColor(startColor, endColor, colorCurve);
        ps->setVelocity(vel);
        ps->setLifeTime(life);
        ps->setSize(startSize, endSize, sizeCurve);
        ps->setSpread(spread);
        ps->setEmissionRate(rate);
        ps->setSpiralSpeed(spiral);
        ps->setGravity(gravity);
        if (pMode == ParticleRenderMode::TEXTURE)
        {
            ps->setTexture(resourceManager->loadTexture(texture));
        }
        ps->init();

        EntityID id = scene->createEntity();
        scene->addParticle(id, {ps});
    }

    if (json.contains("ui"))
    {
        int size = json["ui"].size();
        for (int i = 0; i < size; ++i)
        {
            std::string type = json["ui"][i]["type"];
            int x = json["ui"][i]["x"];
            int y = json["ui"][i]["y"];
            bool vis = json["ui"][i].contains("visible") ? 
                (bool)json["ui"][i]["visible"] : true;
        
            if (type == "label")
            {
                std::string text = json["ui"][i]["text"];
                std::string fntPath = json["ui"][i]["font"];
                std::string texPath = json["ui"][i]["fontTexture"];
                Font* font = resourceManager->loadFont(fntPath, texPath);
                UILabel* label = new UILabel(x, y, text, font);
                label->setVisible(vis);
                uiManager->addElement(label);
                Logger::info("added element.");
            }
            else if (type == "panel")
            {
                int width = json["ui"][i]["width"];
                int height = json["ui"][i]["height"];
                glm::vec3 color = {
                    json["ui"][i]["color"][0],
                    json["ui"][i]["color"][1],
                    json["ui"][i]["color"][2]
                };
                UIPanel* panel = new UIPanel(x, y, width, height, color);
                panel->setVisible(vis);
                uiManager->addElement(panel);
            }
            else if (type == "image")
            {
                int width = json["ui"][i]["width"];
                int height = json["ui"][i]["height"];
                std::string texPath = json["ui"][i]["texture"];
                Texture* tex = resourceManager->loadTexture(texPath);
                UIImage* image = new UIImage(x, y, width, height, tex);
                image->setVisible(vis);
                uiManager->addElement(image);
            }
            else if (type == "button")
            {
                int width = json["ui"][i]["width"];
                int height = json["ui"][i]["height"];
                std::string normalTex = json["ui"][i]["normalTexture"];
                std::string hoverTex = json["ui"][i]["hoverTexture"];
                std::string btnId = json["ui"][i]["id"];
                UIButton* button = new UIButton(x, y, width, height,
                    resourceManager->loadTexture(normalTex),
                    resourceManager->loadTexture(hoverTex),
                    [uiManager = this->uiManager, btnId]() {
                        if (uiManager->callbacks.count(btnId) > 0)
                            uiManager->callbacks[btnId]();
                    }
                );
                button->setVisible(vis);
                uiManager->addElement(button);
            }
        }
    }
}
