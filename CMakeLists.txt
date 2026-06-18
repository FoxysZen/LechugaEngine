cmake_minimum_required(VERSION 3.25)
project(LechugaEngine VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(vendor/SDL3)

# GLM: header-only
add_subdirectory(vendor/glm)

find_package(OpenGL REQUIRED)

add_executable(LechugaEngine
    src/main.cc

    src/application/Application.cc

    src/core/Logger.cc
    src/core/Timer.cc
    src/core/FileSystem.cc
    src/core/ResourceManager.cc
    src/core/OBJParser.cc
    src/core/MTLParser.cc
    src/core/stb_image_impl.cc
    src/core/Config.cc
    src/core/SceneLoader.cc

    src/input/InputManager.cc
    
    src/physics/BoxCollider.cc
    src/physics/CapsuleCollider.cc
    src/physics/CharacterController.cc
    src/physics/MeshCollider.cc
    src/physics/PhysicsEngine.cc
    src/physics/RigidBody.cc
    src/physics/SphereCollider.cc

    src/renderer/Renderer.cc
    src/renderer/ShaderProgram.cc
    src/renderer/Mesh.cc
    src/renderer/Texture.cc
    src/renderer/Frustum.cc
    src/renderer/Skydome.cc
    
    src/scene/Camera.cc
    src/scene/Scene.cc
    src/scene/ParticleSystem.cc
    
    src/ui/Font.cc
    src/ui/FontParser.cc
    src/ui/UIButton.cc
    src/ui/UIImage.cc
    src/ui/UILabel.cc
    src/ui/UIManager.cc
    src/ui/UIPanel.cc
    src/ui/UIRenderer.cc

    src/window/Window.cc

    vendor/glad/src/glad.c
)

target_include_directories(LechugaEngine PRIVATE
    src/
    src/application/
    src/core/
    src/input/
    src/physics/
    src/renderer/
    src/scene/
    src/ui/
    src/window/
)

target_include_directories(LechugaEngine SYSTEM PRIVATE
    vendor/glad/include
    vendor/nlohmann
    vendor/SDL3/include
    vendor/stb
)

target_link_libraries(LechugaEngine PRIVATE
    SDL3::SDL3
    glm::glm
    OpenGL::GL
)

add_custom_command(TARGET LechugaEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:LechugaEngine>/assets
    COMMENT "Copiando assets..."
)

# Warnings
if(MSVC)
    target_compile_options(LechugaEngine PRIVATE /W4)
else()
    target_compile_options(LechugaEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
