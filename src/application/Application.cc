#include <AudioManager.h>
#include <Logger.h>
#include <Application.h>
#include <memory>
#include <string>

#ifndef NDEBUG
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#endif

Application::Application() {}

Application::~Application() {}

bool Application::init()
{
    Logger::info("C++ version: " + std::to_string(__cplusplus));

    Logger::info("Reading config.json");
    Config::getInstance().load("assets/config.json");

    Logger::info("Creating window");
    int width = Config::getInstance().windowWidth;
    int height = Config::getInstance().windowHeight;
    std::string title = Config::getInstance().windowTitle;
    window = std::make_unique<Window>(title, width, height);

    Logger::info("Creating resourceManager");
    resourceManager = std::make_unique<ResourceManager>();
    
    audioManager = std::make_unique<AudioManager>();
    audioManager->init();
    audioManager->setVolume(AudioChannel::MUSIC, 0.7f);
    audioManager->setVolume(AudioChannel::SFX,   0.5f);
    audioManager->setVolume(AudioChannel::UI,    0.6f);

    Logger::info("Creating renderer");
    renderer = std::make_unique<Renderer>(width, height);

    Logger::info("Creating physics engine");
    physicsEngine = std::make_unique<PhysicsEngine>();

    skydome = std::make_unique<Skydome>();
    skydome->init(
        resourceManager->loadShader(Config::getInstance().skydomeVertShader, 
            Config::getInstance().skydomeFragShader),
        resourceManager->loadTexture(Config::getInstance().skydomeTexture)
    );
    renderer->setSkydome(skydome.get());

    Logger::info("Creating camera");
    float fov = Config::getInstance().cameraFov;
    float zN = Config::getInstance().cameraNear;
    float zF = Config::getInstance().cameraFar;
    camera = std::make_unique<Camera>(width, height, fov, zN, zF);

    input = std::make_unique<InputManager>();
    timer = std::make_unique<Timer>();
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
    sceneLoader = std::make_unique<SceneLoader>(scene.get(), 
        resourceManager.get(), uiManager.get(), physicsEngine.get(), 
        audioManager.get());
    sceneLoader->loadScene("testScene.json");

    Logger::info("Creating CHaracterController");
    characterController = std::make_unique<CharacterController>(
            scene->getIdByName("player"), physicsEngine.get(), scene.get());

    Font *font = resourceManager->loadFont("assets/fonts/SansSerif.fnt",
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
#ifndef NDEBUG
        if (ImGui::GetIO().WantCaptureMouse || ImGuizmo::IsOver() || ImGuizmo::IsUsing()) 
            return;
#endif

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

#ifndef NDEBUG
    Logger::info("Initializing Dear ImGui Editor...");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window->getWindow(), window->getGLContext());
    ImGui_ImplOpenGL3_Init("#version 410 core");
#endif
    
    Logger::info("Init complete");
    return window->isOpen();
}

void Application::run()
{
    Logger::info("Starting run loop...");

    // Temporal
    audioManager->playMusic("assets/audio/music/town.mp3");

    int frames = 0;
    float elapsed = 0.0f;

#ifndef NDEBUG
    static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
    static ImGuizmo::MODE currentGizmoMode = ImGuizmo::LOCAL;
#endif

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

#ifndef NDEBUG
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        ImGuizmo::Enable(true);
#endif

    ///////// dear ImGui
        bool ignoreGameInput = false;
#ifndef NDEBUG
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantCaptureKeyboard ||
            ImGuizmo::IsUsing() || ImGuizmo::IsOver())
        {
            ignoreGameInput = true;
        }

        if (!io.WantCaptureKeyboard)
        {
            if (input->isKeyDown(SDLK_1))
                currentGizmoOperation = ImGuizmo::TRANSLATE;
            if (input->isKeyDown(SDLK_2))
                currentGizmoOperation = ImGuizmo::ROTATE;
            if (input->isKeyDown(SDLK_3))
                currentGizmoOperation = ImGuizmo::SCALE;
        }
#endif

    ///////// character movement controler
        glm::vec3 direction = glm::vec3(0.0f);
        if (!ignoreGameInput)
        {
            glm::vec3 forward = glm::normalize(glm::vec3(camera->getForward().x,
                                               0.0f, camera->getForward().z));
            glm::vec3 right = glm::normalize(glm::vec3(camera->getRight().x,
                                             0.0f, camera->getRight().z));

            if (input->isKeyDown(SDLK_UP)) direction += forward;
            if (input->isKeyDown(SDLK_DOWN)) direction -= forward;
            if (input->isKeyDown(SDLK_LEFT)) direction -= right;
            if (input->isKeyDown(SDLK_RIGHT)) direction += right;
            if (glm::length(direction) > 0.0f) direction = glm::normalize(direction);
        }

        characterController->move(direction, 5.0f, deltaTime); // TODO: speed not hardcoded
    /////////

        physicsEngine->step(deltaTime, scene.get());

#ifndef NDEBUG
        if (!ImGuizmo::IsUsing())
        {
            camera->update(input.get(), deltaTime);
        }
#else
        camera->update(input.get(), deltaTime);
#endif

        renderer->beginFrame(camera.get(), deltaTime);

        scene->update(deltaTime);

        scene->render(renderer.get());

        uiManager->handleHover(input->getMouseX(), input->getMouseY());
        uiManager->draw();

        // Debug Hitboxes
        if (debugRenderer->isVisible())
        {
            glm::mat4 viewProj = 
                camera->getProjectionMatrix() * camera->getViewMatrix();
            for (auto& [id, collider] : *scene->getColliderMap())
            {
                TransformComponent* t = scene->getTransform(id);
                glm::vec3 color = glm::vec3(0, 1, 0);
                debugRenderer->draw(collider.collider, t, viewProj, color);
                renderer->addDrawCalls(1);
            }
            for (auto& [id, collider] : *scene->getTriggerColliderMap())
            {
                TransformComponent* t = scene->getTransform(id);
                glm::vec3 color = glm::vec3(0, 0, 1);
                debugRenderer->draw(collider.collider, t, viewProj, color);
                renderer->addDrawCalls(1);
            }
        }

#ifndef NDEBUG
    //////// Render Dear ImGui
        ImGui::Begin("Scene Hierarchy");
        auto *meshesMap = scene->getSkinnedMeshMap();
        if (meshesMap)
        {
            for (auto &[id, mesh] : *meshesMap)
            {
                std::string name = scene->getNameById(id) + " ID: " + std::to_string(id);

                bool isSelected = (objSelected && selectedEntityIdx == id);
                if (ImGui::Selectable(name.c_str(), isSelected))
                {
                    selectedEntityIdx = id;
                    objSelected = true;
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Inspector");
        if (objSelected)
        {
            TransformComponent *t = scene->getTransform(selectedEntityIdx);
            if (t)
            {
                ImGui::Text("Entity %u", selectedEntityIdx);

                if (ImGui::RadioButton("Translate (1)", currentGizmoOperation == ImGuizmo::TRANSLATE))
                    currentGizmoOperation = ImGuizmo::TRANSLATE;
                ImGui::SameLine();
                if (ImGui::RadioButton("Rotate (2)", currentGizmoOperation == ImGuizmo::ROTATE))
                    currentGizmoOperation = ImGuizmo::ROTATE;
                ImGui::SameLine();
                if (ImGui::RadioButton("Scale (3)", currentGizmoOperation == ImGuizmo::SCALE))
                    currentGizmoOperation = ImGuizmo::SCALE;

                ImGui::Separator();

                ImGui::DragFloat3("Position", &t->position.x, 0.05f);
                ImGui::DragFloat3("Rotation", &t->rotation.x, 0.5f);
                ImGui::DragFloat3("Scale", &t->scale.x, 0.01f);

                ImGui::Separator();
            
                if (ImGui::Button("Save Scene (JSON)", ImVec2(-1, 30)))
                {
                    sceneLoader->saveScene("assets/scenes/testScene.json");
                    Logger::info("Scene Saved!");
                }
            }
        }
        else
        {
            ImGui::Text("Select Object from Hierarchy");
        }
        ImGui::End();

        if (objSelected)
        {
            TransformComponent *t = scene->getTransform(selectedEntityIdx);
            if (t && camera)
            {
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
            
                ImGuiIO &io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            
                glm::mat4 view = camera->getViewMatrix();
                glm::mat4 proj = camera->getProjectionMatrix();
            
                ImGuizmo::MODE mode = (currentGizmoOperation == ImGuizmo::SCALE) ? ImGuizmo::LOCAL : currentGizmoMode;
            
                glm::mat4 matrix;
                ImGuizmo::RecomposeMatrixFromComponents(
                    glm::value_ptr(t->position),
                    glm::value_ptr(t->rotation),
                    glm::value_ptr(t->scale),
                    glm::value_ptr(matrix)
                );
            
                ImGuizmo::Manipulate(
                    glm::value_ptr(view),
                    glm::value_ptr(proj),
                    currentGizmoOperation,
                    mode,
                    glm::value_ptr(matrix)
                );
            
                if (ImGuizmo::IsUsing())
                {
                    ImGuizmo::DecomposeMatrixToComponents(
                        glm::value_ptr(matrix),
                        glm::value_ptr(t->position),
                        glm::value_ptr(t->rotation),
                        glm::value_ptr(t->scale)
                    );
                }
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        renderer->endFrame();
        window->swapBuffers();
        input->updateMouseLast();
    }
}

void Application::shutdown()
{
#ifndef NDEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif

    delete debugLabel;
}
