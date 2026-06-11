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

    /*Logger::info("Creating light...");
    EntityID light = scene->createEntity();
    scene->addLight(light, {glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f});
    scene->addLight(scene->createEntity(), {glm::vec3(-5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f});

    // Test object
    Logger::info("Creating entity...");
    Logger::info("Loading shader...");
    ShaderProgram* shader = resourceManager->loadShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    Logger::info("Shader loaded");
    EntityID entity = scene->createEntity();
    Logger::info("Entity created");
    scene->addTransform(entity, {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)});
    Logger::info("Transform added");
    Mesh* meshLod0 = resourceManager->loadMesh("assets/meshes/character_lod0.obj");
    Mesh* meshLod1 = resourceManager->loadMesh("assets/meshes/character_lod1.obj");
    Mesh* meshLod2 = resourceManager->loadMesh("assets/meshes/character_lod2.obj");
    Texture* tex0 = resourceManager->loadTexture("assets/textures/char_body_texture.png");
    Texture* tex1 = resourceManager->loadTexture("assets/textures/char_face_texture.png");
    scene->addMesh(entity, {
        {
            {meshLod0, 10.0f},
            {meshLod1, 50.0f},
            {meshLod2, FLT_MAX}
        },
        shader,
        {tex0, tex1}
    });
    Logger::info("Mesh added");*/

    Logger::info("CreatingScene");
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

        scene->render(renderer.get());

        window->swapBuffers();
    }
}

void Application::shutdown()
{
    
}
