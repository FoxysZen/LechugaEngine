#include <Application.h>

Application::Application() {}

Application::~Application() {}

bool Application::init()
{
    Logger::info("Creating window...");
    window = std::make_unique<Window>("LechugaEngine", 400, 240);
    Logger::info("Creating resourceManager...");
    resourceManager = std::make_unique<ResourceManager>();
    Logger::info("Creating renderer...");
    renderer = std::make_unique<Renderer>();
    Logger::info("Creating camera...");
    camera = std::make_unique<Camera>(400, 240);
    Logger::info("Creating input...");
    input = std::make_unique<InputManager>();
    timer = std::make_unique<Timer>();
    Logger::info("Creating scene...");
    scene = std::make_unique<Scene>();

    // Test object
    Logger::info("Creating entity...");
    Logger::info("Loading shader...");
    ShaderProgram* shader = resourceManager->loadShader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    Logger::info("Shader loaded");
    EntityID entity = scene->createEntity();
    Logger::info("Entity created");
    scene->addTransform(entity, {glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)});
    Logger::info("Transform added");
    Mesh* mesh = resourceManager->loadMesh("assets/meshes/character.obj");
    Texture* tex0 = resourceManager->loadTexture("assets/textures/char_body_texture.png");
    Texture* tex1 = resourceManager->loadTexture("assets/textures/char_face_texture.png");
    scene->addMesh(entity, {mesh, shader, {tex0, tex1}});
    Logger::info("Mesh added");
    
    Logger::info("Init complete");
    return window->isOpen();
}

void Application::run()
{
    Logger::info("Starting run loop...");
    while (window->isOpen())
    {
        timer->tick();
        float deltaTime = timer->getDeltaTime();

        if (!input->pollEvents())
        {
            window->closeWindow();
        }

        if (input->wasWindowResized())
        {
            renderer->onResize(input->getNewWidth(), input->getNewHeight());
            camera->setAspectRatio(input->getNewWidth(), input->getNewHeight());
            input->resetWindowResized();
        }

        camera->update(input.get(), deltaTime);
        renderer->beginFrame(camera.get());

        scene->render(renderer.get());

        window->swapBuffers();
    }
}

void Application::shutdown()
{
    
}
