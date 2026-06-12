#include <Application.h>

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
    Logger::info("Creating renderer...");
    renderer = std::make_unique<Renderer>();

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

    Logger::info("Loading Scene");
    sceneLoader = std::make_unique<SceneLoader>(scene.get(), resourceManager.get());
    sceneLoader->loadScene("testScene.json");

    Logger::info("Adding subscriptions");
    // Cerrar la ventana cuando se publique QuitEvent
    EventSystem::getInstance().subscribe<QuitEvent>([this](const QuitEvent& e) {
        window->closeWindow();
    });

    // Actualizar renderer y camera cuando se redimensione
    EventSystem::getInstance().subscribe<WindowResizedEvent>([this](const WindowResizedEvent& e) {
        renderer->onResize(e.width, e.height);
        camera->setAspectRatio(e.width, e.height);
    });

    EventSystem::getInstance().subscribe<LeftMousePressedEvent>([this](const LeftMousePressedEvent& e) {
        camera->setLeftMouse(true);
        input->captureMousePosition();
    });

    EventSystem::getInstance().subscribe<LeftMouseReleasedEvent>([this](const LeftMouseReleasedEvent& e) {
        camera->setLeftMouse(false);
    });
    
    Logger::info("Init complete");
    return window->isOpen();
}

void Application::run()
{
    Logger::info("Starting run loop...");
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
            window->setTitle("LechugaEngine | FPS: " + std::to_string(frames));
            frames = 0;
            elapsed = 0.0f;
        }

        input->pollEvents();

        camera->update(input.get(), deltaTime);
        renderer->beginFrame(camera.get(), deltaTime);

        scene->update(deltaTime);

        scene->render(renderer.get());

        window->swapBuffers();
        input->updateMouseLast();
    }
}

void Application::shutdown()
{
    
}
