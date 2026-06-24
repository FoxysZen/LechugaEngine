#include "AudioManager.h"
#include "Logger.h"
#include <Application.h>
#include <memory>
#include <string>

Application::Application() {}

Application::~Application() {}

bool Application::init()
{
    Logger::info("Reading config.json");
    Config::getInstance().load("assets/config.json");

    Logger::info("Creating window...");
    int width = Config::getInstance().windowWidth;
    int height = Config::getInstance().windowHeight;
    std::string title = Config::getInstance().windowTitle;
    window = std::make_unique<Window>(title, width, height);

    Logger::info("Creating resourceManager...");
    resourceManager = std::make_unique<ResourceManager>();
    
    audioManager = std::make_unique<AudioManager>();
    audioManager->init();
    audioManager->setVolume(AudioChannel::MUSIC, 0.7f);
    audioManager->setVolume(AudioChannel::SFX,   0.5f);
    audioManager->setVolume(AudioChannel::UI,    0.6f);

    Logger::info("Creating renderer...");
    renderer = std::make_unique<Renderer>();

    Logger::info("Creating physics engine...");
    physicsEngine = std::make_unique<PhysicsEngine>();

    Logger::info("Creating skydome...");
    skydome = std::make_unique<Skydome>();
    skydome->init(
        resourceManager->loadShader(Config::getInstance().skydomeVertShader, 
            Config::getInstance().skydomeFragShader),
        resourceManager->loadTexture(Config::getInstance().skydomeTexture)
    );
    renderer->setSkydome(skydome.get());

    Logger::info("Creating camera...");
    float fov = Config::getInstance().cameraFov;
    float zN = Config::getInstance().cameraNear;
    float zF = Config::getInstance().cameraFar;
    camera = std::make_unique<Camera>(width, height, fov, zN, zF);
    Logger::info("Creating input...");
    input = std::make_unique<InputManager>();
    timer = std::make_unique<Timer>();
    Logger::info("Creating scene...");
    scene = std::make_unique<Scene>();

    Logger::info("Loading UI");
    uiRenderer = std::make_unique<UIRenderer>();
    uiRenderer->init(
        resourceManager->loadShader("assets/shaders/ui.vert",
                                    "assets/shaders/ui.frag"),
        Config::getInstance().windowWidth,
        Config::getInstance().windowHeight
    );
    uiManager = std::make_unique<UIManager>();
    uiManager->init(uiRenderer.get());

    Logger::info("Loading Scene");
    auto sceneLoader = std::make_unique<SceneLoader>(scene.get(), 
        resourceManager.get(), uiManager.get(), physicsEngine.get());
    sceneLoader->loadScene("testScene.json");

    Logger::info("Creating CHaracterController");
    characterController = std::make_unique<CharacterController>(0, 
                                            physicsEngine.get(), scene.get());

    Font *font = new Font();
    font = resourceManager->loadFont("assets/fonts/SansSerif.fnt",
                                     "assets/fonts/SansSerif.png");
    debugLabel = new UILabel(10, 10, "", font);
    debugLabel->setVisible(false);
    debugLabel->setLayer(10);
    debugLabel->setScale(0.5f);
    uiManager->addElement(debugLabel);

    debugRenderer = std::make_unique<DebugRenderer>();
    debugRenderer->init();

    uiManager->registerCallback("btnPlay", [this]() {
        Logger::info("Play pulsado");
        audioManager->playSFX("assets/audio/sfx/button.mp3");
    });

    Logger::info("Adding subscriptions");
    EventSystem::getInstance().subscribe<QuitEvent>([this](const QuitEvent&) {
        window->closeWindow();
    });

    EventSystem::getInstance().subscribe<WindowResizedEvent>([this](const WindowResizedEvent& e) {
        renderer->onResize(e.width, e.height);
        camera->setAspectRatio(e.width, e.height);
    });

    EventSystem::getInstance().subscribe<LeftMousePressedEvent>([this](const LeftMousePressedEvent&) {
        if (!uiManager->handleClick(input->getMouseX(), input->getMouseY()))
        {
            camera->setLeftMouse(true);
            input->captureMousePosition();
        }
    });

    EventSystem::getInstance().subscribe<KeyPressedEvent>([this](const KeyPressedEvent& e) {
        if (e.key == SDLK_F3)
        {
            debugLabel->setVisible(!debugLabel->isVisible());
            debugRenderer->setVisible(!debugRenderer->isVisible());
        }
    });

    EventSystem::getInstance().subscribe<LeftMouseReleasedEvent>([this](const LeftMouseReleasedEvent&) {
        camera->setLeftMouse(false);
    });
    
    Logger::info("Init complete");
    return window->isOpen();
}

void Application::run()
{
    Logger::info("Starting run loop...");
    audioManager->playMusic("assets/audio/music/town.mp3");
    int frames = 0;
    float elapsed = 0.0f;
    while (window->isOpen())
    {
        timer->tick();
        float deltaTime = timer->getDeltaTime();

        frames++;
        elapsed += deltaTime;
        if (elapsed >= 1.0f)
        {
            debugStats = "FPS: " + std::to_string(frames) + "\nDraw calls: " + 
                        std::to_string(renderer->getDrawCalls() / frames);
            renderer->resetDrawCalls();
            frames = 0;
            elapsed = 0.0f;
        }
        if (debugLabel->isVisible())
        {
            glm::vec3 camPos = camera->getPosition();
            std::string coords = "\nCam Coords: X: " + std::to_string(camPos.x)
                               + " Y: " + std::to_string(camPos.y) +
                                 " Z: " + std::to_string(camPos.z);
            debugLabel->setText(debugStats + coords);
        }

        input->pollEvents();

        glm::vec3 forward = glm::normalize(glm::vec3(camera->getForward().x,
                                           0.0f, camera->getForward().z));
        glm::vec3 right = glm::normalize(glm::vec3(camera->getRight().x,
                                         0.0f, camera->getRight().z));

        glm::vec3 direction = glm::vec3(0.0f);
        if (input->isKeyDown(SDLK_UP)) direction += forward;
        if (input->isKeyDown(SDLK_DOWN)) direction -= forward;
        if (input->isKeyDown(SDLK_LEFT)) direction -= right;
        if (input->isKeyDown(SDLK_RIGHT)) direction += right;
        if (glm::length(direction) > 0.0f) direction = glm::normalize(direction);

        characterController->move(direction, 5.0f, deltaTime);

        physicsEngine->step(deltaTime, scene.get());

        camera->update(input.get(), deltaTime);
        renderer->beginFrame(camera.get(), deltaTime);

        scene->update(deltaTime);

        scene->render(renderer.get());

        uiManager->handleHover(input->getMouseX(), input->getMouseY());
        uiManager->draw();

        // Debug Hitboxes
        if (debugRenderer->isVisible())
        {
            glm::mat4 viewProj = camera->getProjectionMatrix() * camera->getViewMatrix();
            for (auto& [id, collider] : *scene->getColliderMap())
            {
                TransformComponent* t = scene->getTransform(id);
                glm::vec3 color = glm::vec3(0, 1, 0);
                debugRenderer->draw(collider.collider, t, viewProj, color);
                renderer->addDrawCalls(1);
            }
        }

        window->swapBuffers();
        input->updateMouseLast();
    }
}

void Application::shutdown()
{
    delete debugLabel;
}
