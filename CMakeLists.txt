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
    src/main.cpp

    #src/core/Logger.cpp
    #src/core/Timer.cpp
    #src/core/FileSystem.cpp

    #src/window/Window.cpp
    #src/input/InputManager.cpp

    #src/renderer/Renderer.cpp
    #src/renderer/ShaderProgram.cpp
    #src/renderer/Mesh.cpp

    vendor/glad/src/glad.c
)

target_include_directories(LechugaEngine PRIVATE
    src/
    vendor/glad/include
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
