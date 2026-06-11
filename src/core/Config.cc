#include <Config.h>

void Config::load(const std::string& path)
{
    std::string file = FileSystem::readFile(path);
    nlohmann::json json = nlohmann::json::parse(file);

    windowWidth      = json["window"]["width"];
    windowHeight     = json["window"]["height"];
    windowTitle      = json["window"]["title"];

    cameraFov         = json["camera"]["fov"];
    cameraNear        = json["camera"]["near"];
    cameraFar         = json["camera"]["far"];
    cameraSpeed       = json["camera"]["speed"];
    cameraSensitivity = json["camera"]["sensitivity"];

    clearColor = glm::vec4(
        json["renderer"]["clearColor"][0],
        json["renderer"]["clearColor"][1],
        json["renderer"]["clearColor"][2],
        json["renderer"]["clearColor"][3]
    );
}