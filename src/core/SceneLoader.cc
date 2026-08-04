#include "ColliderType.h"
#include "ParticleSystem.h"
#include "SphereCollider.h"
#include <SceneLoader.h>
#include <fstream>


SceneLoader::SceneLoader(Scene *_scene, ResourceManager *_resourceManager,
    UIManager *_uiManager, PhysicsEngine *_physicsEngine)
{
    scene = _scene;
    resourceManager = _resourceManager;
    uiManager = _uiManager;
    physicsEngine = _physicsEngine;
}

SceneLoader::~SceneLoader() {}

void SceneLoader::loadScene(const std::string &sceneName)
{
    std::string file = FileSystem::readFile("assets/scenes/" + sceneName);
    nlohmann::json json = nlohmann::json::parse(file);

    int size = json["entities"].size();
    for (int i = 0; i < size; ++i)
    {
        Logger::info("Loading entity " + std::to_string(i));
        EntityID id = scene->createEntity();
Logger::info("Loading transform...");
        glm::vec3 position = glm::vec3(
            json["entities"][i]["transform"]["position"][0],
            json["entities"][i]["transform"]["position"][1],
            json["entities"][i]["transform"]["position"][2]
        );
        Logger::info("position: " + std::to_string(position.x) + " " + 
             std::to_string(position.y) + " " + std::to_string(position.z));

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
Logger::info("Loading mesh...");
        if (json["entities"][i].contains("skinnedMesh"))
        {
            std::vector<SkinnedLOD> lods;
            if (json["entities"][i]["skinnedMesh"].contains("lods"))
            {
                int nLods = json["entities"][i]["skinnedMesh"]["lods"].size();
                lods.resize(nLods);
                for (int j = 0; j < nLods; ++j)
                {
                    lods[j].mesh = resourceManager->loadSkinnedMesh(
                        json["entities"][i]["skinnedMesh"]["lods"][j]["path"]);
                    lods[j].maxDistance = 
                        json["entities"][i]["skinnedMesh"]["lods"][j]["maxDistance"];
                }
            }
            else if (json["entities"][i]["skinnedMesh"].contains("path"))
            {
                lods.push_back({
                    resourceManager->loadSkinnedMesh(json["entities"][i]["skinnedMesh"]["path"]),
                    10000.0f
                });
            }

            std::string vert = json["entities"][i]["skinnedMesh"]["shader"][0];
            std::string frag = json["entities"][i]["skinnedMesh"]["shader"][1];
            bool loop = json["entities"][i]["skinnedMesh"].value("loop", true);
        
            ShaderProgram *shader = resourceManager->loadShader(vert, frag);
        
            AnimationSystem *animSys = new AnimationSystem();
            if (!lods.empty())
            {
                animSys->setMesh(lods[0].mesh);
            }

            if (json["entities"][i]["skinnedMesh"].contains("animation"))
            {
                std::string anim = json["entities"][i]["skinnedMesh"]["animation"];
                animSys->play(anim, loop);
            }
        
            scene->addSkinnedMesh(id, {lods, shader, animSys});
        }

        if (json["entities"][i].contains("collider"))
        {
            std::string colType = json["entities"][i]["collider"]["type"];
            Collider *collider = nullptr;

            glm::vec3 offset = glm::vec3(0.0f);
            if (json["entities"][i]["collider"].contains("offset"))
            {
                offset = glm::vec3(
                    json["entities"][i]["collider"]["offset"][0],
                    json["entities"][i]["collider"]["offset"][1],
                    json["entities"][i]["collider"]["offset"][2]
                );
            }

            if (colType == "SPHERE")
            {
                float radius = json["entities"][i]["collider"]["radius"];
                collider = new SphereCollider(radius, offset);
            }
            else if (colType == "CAPSULE")
            {
                float radius = json["entities"][i]["collider"]["radius"];
                float height = json["entities"][i]["collider"]["height"];
                collider = new CapsuleCollider(radius, height);
                collider->offset = offset;
            }
            else if (colType == "BOX")
            {
                glm::vec3 halfExtents = glm::vec3(
                    json["entities"][i]["collider"]["halfExtents"][0],
                    json["entities"][i]["collider"]["halfExtents"][1],
                    json["entities"][i]["collider"]["halfExtents"][2]
                );
                collider = new BoxCollider(halfExtents);
                collider->offset = offset;
            }
            else if (colType == "MESH")
            {
                std::string path = json["entities"][i]["collider"]["path"];
                std::vector<glm::vec3> triangles;
            
                glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
                model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
                model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
                model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
                model = glm::scale(model, scale);
            
                if (path.substr(path.find_last_of(".") + 1) == "glb")
                {
                    cgltf_options options = {};
                    cgltf_data *data = nullptr;
                
                    if (cgltf_parse_file(&options, path.c_str(), &data) == cgltf_result_success &&
                        cgltf_load_buffers(&options, data, path.c_str()) == cgltf_result_success)
                    {
                        for (size_t mi = 0; mi < data->meshes_count; ++mi)
                        {
                            const cgltf_mesh &mesh = data->meshes[mi];
                            for (size_t pi = 0; pi < mesh.primitives_count; ++pi)
                            {
                                const cgltf_primitive &prim = mesh.primitives[pi];

                                std::vector<glm::vec3> positions;
                                for (size_t ai = 0; ai < prim.attributes_count; ++ai)
                                {
                                    const cgltf_attribute &attr = prim.attributes[ai];
                                    if (attr.type == cgltf_attribute_type_position)
                                    {
                                        positions.resize(attr.data->count);
                                        for (size_t v = 0; v < attr.data->count; ++v)
                                        {
                                            cgltf_accessor_read_float(attr.data, 
                                                v, 
                                                (float*)&positions[v], 
                                                3);
                                        }
                                        break;
                                    }
                                }
                            
                                if (prim.indices && !positions.empty())
                                {
                                    size_t indexCount = prim.indices->count;
                                    for (size_t ii = 0; ii < indexCount; ++ii)
                                    {
                                        uint32_t idx = 
                                            (uint32_t)cgltf_accessor_read_index(prim.indices, ii);
                                        glm::vec4 worldPos = 
                                            model * glm::vec4(positions[idx], 1.0f);
                                        triangles.push_back(glm::vec3(worldPos));
                                    }
                                }
                            }
                        }
                        cgltf_free(data);
                    }
                    else
                    {
                        Logger::error("SceneLoader: Failed to load GLB collider: " + path);
                    }
                }
            
                collider = new MeshCollider(triangles);
            }

            if (collider)
            {
                scene->addCollider(id, {collider});
                physicsEngine->addCollider(id, collider);
            }
        }

        if (json["entities"][i].contains("rigidBody"))
        {
            float mass = json["entities"][i]["rigidBody"]["mass"];
            bool useGravity = json["entities"][i]["rigidBody"]["useGravity"];
            bool isKinematic = json["entities"][i]["rigidBody"]["isKinematic"];

            RigidBody *body = new RigidBody();
            body->init(glm::vec3(0.0f), glm::vec3(0.0f), mass, useGravity, 
                       isKinematic);
            scene->addRigidBody(id, body);
            physicsEngine->addBody(id, body);
        }

        if (json["entities"][i].contains("material"))
        {
            bool cellShaded = json["entities"][i]["material"]["cellShaded"];
            bool foliage = json["entities"][i]["material"]["foliage"];

            glm::vec3 diffuse = glm::vec3(
                json["entities"][i]["material"]["diffuse"][0],
                json["entities"][i]["material"]["diffuse"][1],
                json["entities"][i]["material"]["diffuse"][2]
            );
            glm::vec3 specular = glm::vec3(
                json["entities"][i]["material"]["specular"][0],
                json["entities"][i]["material"]["specular"][1],
                json["entities"][i]["material"]["specular"][2]
            );
            float shin = json["entities"][i]["material"]["shin"];

            
            scene->addMaterial(id, {cellShaded, foliage, diffuse, specular, shin});
        }
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

void SceneLoader::saveScene(const std::string &path)
{
    nlohmann::json rootJson;
    std::ifstream inFile(path);
    if (!inFile.is_open()) return;
    
    try
    {
        inFile >> rootJson;
    }
    catch (...)
    {
        Logger::error("Could not save the scene: Empty or corrupted file.");
        inFile.close();
        return;
    }
    inFile.close();

    auto *transformMap = scene->getTransformMap();
    auto *colliderMap = scene->getColliderMap();
    auto *rbMap = scene->getRigidBodyMap();

    unsigned int globalEntityId = 0;

    if (transformMap && rootJson.contains("entities"))
    {
        for (auto &entityJson : rootJson["entities"])
        {
            unsigned int entityId = globalEntityId++;
            
            auto transformIt = transformMap->find(entityId);
            if (transformIt != transformMap->end())
            {
                const auto &transform = transformIt->second;
                entityJson["transform"]["position"] = {
                    transform.position.x,
                    transform.position.y,
                    transform.position.z
                };
                entityJson["transform"]["rotation"] = {
                    transform.rotation.x,
                    transform.rotation.y,
                    transform.rotation.z
                };
                entityJson["transform"]["scale"] = {
                    transform.scale.x,
                    transform.scale.y,
                    transform.scale.z
                };
            }

            if (colliderMap)
            {
                auto colIt = colliderMap->find(entityId);
                if (colIt != colliderMap->end())
                {
                    const auto &col = colIt->second;
                    entityJson["collider"]["offset"] = {
                        col.collider->offset.x,
                        col.collider->offset.y,
                        col.collider->offset.z
                    };

                    if (col.collider->getType() == ColliderType::BOX)
                    {
                        auto *box = static_cast<BoxCollider*>(col.collider);
                        glm::vec3 halfExtents = box->getHalfExtents();
                        entityJson["collider"]["halfExtents"] = { 
                            halfExtents.x, 
                            halfExtents.y, 
                            halfExtents.z
                        };
                    }
                    else if (col.collider->getType() == ColliderType::SPHERE)
                    {
                        auto *sph = static_cast<SphereCollider*>(col.collider);
                        entityJson["collider"]["radius"] = sph->getRadius();
                    }
                    else if (col.collider->getType() == ColliderType::CAPSULE)
                    {
                        auto *cap = static_cast<CapsuleCollider*>(col.collider);
                        entityJson["collider"]["radius"] = cap->getRadius();
                        entityJson["collider"]["height"] = cap->getHeight();
                    }
                }
            }

            if (rbMap)
            {
                auto rbIt = rbMap->find(entityId);
                if (rbIt != rbMap->end())
                {
                    const auto &rb = rbIt->second;
                    entityJson["rigidBody"]["mass"] = rb->getMass();
                    entityJson["rigidBody"]["useGravity"] = rb->getUseGravity();
                    entityJson["rigidBody"]["isKinematic"] = rb->getIsKinematic();
                }
            }
        }
    }

    if (rootJson.contains("lights"))
    {
        //size_t index = 0;
        //for (const auto &light : *scene->getLights())
        //{
        //    if (index >= rootJson["lights"].size()) break;
        //    auto &lightJson = rootJson["lights"][index++];
        //    lightJson["position"] = {
        //        light.second.position.x,
        //        light.second.position.y,
        //        light.second.position.z
        //    };
        //    lightJson["color"] = {
        //        light.second.color.r,
        //        light.second.color.g,
        //        light.second.color.b
        //    };
        //    lightJson["intensity"] = light.second.intensity;
        //}
        for (auto &lightJson : rootJson["lights"])
        {
            unsigned int lightEntityId = globalEntityId++;

            auto lightsMap = scene->getLights();
            auto lightIt = lightsMap->find(lightEntityId);
            if (lightIt != lightsMap->end())
            {
                const auto &light = lightIt->second;
                lightJson["position"] = {
                    light.position.x,
                    light.position.y,
                    light.position.z
                };
                lightJson["color"] = {
                    light.color.r,
                    light.color.g,
                    light.color.b
                };
                lightJson["intensity"] = light.intensity;
            }
        }
    }

    if (rootJson.contains("particles"))
    {
        //size_t index = 0;
        //for (const auto &component : *scene->getParticlesMap())
        //{
        //    if (index >= rootJson["particles"].size()) break;
        //    ParticleSystem *particle = component.second.system;
        //    auto &pJson = rootJson["particles"][index++];
//
        //    const auto &pos = particle->getPosition();
        //    const auto &dir = particle->getDirection();
        //    const auto &sCol = particle->getStartColor();
        //    const auto &eCol = particle->getEndColor();
//
        //    pJson["position"] = {
        //        pos.x,
        //        pos.y,
        //        pos.z
        //    };
        //    pJson["direction"] = {
        //        dir.x,
        //        dir.y,
        //        dir.z
        //    };
        //    pJson["startColor"] = {
        //        sCol.r,
        //        sCol.g,
        //        sCol.b
        //    };
        //    pJson["endColor"] = {
        //        eCol.r,
        //        eCol.g,
        //        eCol.b
        //    };
//
        //    pJson["velocity"] = particle->getVelocity();
        //    pJson["lifeTime"] = particle->getLifeTime();
        //    pJson["startSize"] = particle->getStartSize();
        //    pJson["endSize"] = particle->getEndSize();
        //    pJson["sizeCurve"] = particle->getSizeCurve();
        //    pJson["colorCurve"] = particle->getColorCurve();
        //    pJson["spread"] = particle->getSpread();
        //    pJson["emissionRate"] = particle->getEmissionRate();
        //    pJson["spiralSpeed"] = particle->getSpiralSpeed();
        //    pJson["gravity"] = particle->getGravity();
        //    pJson["maxParticles"] = particle->getMaxParticles();
        //}
        for (auto &pJson : rootJson["particles"])
        {
            unsigned int particleEntityId = globalEntityId++;

            auto particlesMap = scene->getParticlesMap();
            auto partIt = particlesMap->find(particleEntityId);
            if (partIt != particlesMap->end())
            {
                ParticleSystem *particle = partIt->second.system;
                if (!particle) continue;

                const auto &pos = particle->getPosition();
                const auto &dir = particle->getDirection();
                const auto &sCol = particle->getStartColor();
                const auto &eCol = particle->getEndColor();

                pJson["position"] = {
                    pos.x,
                    pos.y,
                    pos.z
                };
                pJson["direction"] = {
                    dir.x,
                    dir.y,
                    dir.z
                };
                pJson["startColor"] = {
                    sCol.r,
                    sCol.g,
                    sCol.b
                };
                pJson["endColor"] = {
                    eCol.r, eCol.g,
                    eCol.b
                };

                pJson["velocity"] = particle->getVelocity();
                pJson["lifeTime"] = particle->getLifeTime();
                pJson["startSize"] = particle->getStartSize();
                pJson["endSize"] = particle->getEndSize();
                pJson["sizeCurve"] = particle->getSizeCurve();
                pJson["colorCurve"] = particle->getColorCurve();
                pJson["spread"] = particle->getSpread();
                pJson["emissionRate"] = particle->getEmissionRate();
                pJson["spiralSpeed"] = particle->getSpiralSpeed();
                pJson["gravity"] = particle->getGravity();
                pJson["maxParticles"] = particle->getMaxParticles();
            }
        }
    }

    if (rootJson.contains("ui"))
    {
        //size_t index = 0;
        //for (const auto &element : *uiManager->getElements())
        //{
        //    if (index >= rootJson["ui"].size()) break;
        //    auto &uiJson = rootJson["ui"][index++];
        //    glm::vec4 param = element->getParameters();
        //    uiJson["x"] = param.x;
        //    uiJson["y"] = param.y;
        //    uiJson["width"] = param.z;
        //    uiJson["height"] = param.w;
        //    uiJson["visible"] = element->isVisible();
        //}
        const auto &elements = *uiManager->getElements();
        size_t uiIndex = 0;

        for (auto &uiJson : rootJson["ui"])
        {
            ++globalEntityId;

            if (uiIndex >= elements.size()) break;

            auto *element = elements[uiIndex++];
            if (!element) continue;

            glm::vec4 param = element->getParameters();
            uiJson["x"] = param.x;
            uiJson["y"] = param.y;
            uiJson["width"] = param.z;
            uiJson["height"] = param.w;
            uiJson["visible"] = element->isVisible();
        }
    }

    std::ofstream outFile(path);
    if (outFile.is_open())
    {
        outFile << rootJson.dump(4);
    }
}
