#pragma once
#include "../../vendor/nlohmann/json.hpp"
#include <FileSystem.h>
#include <glm/glm.hpp>
#include <string>

class Config
{
    public:
        static Config& getInstance()
        {
            static Config instance;
            return instance;
        }
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

        void load(const std::string& path);

        int windowWidth;
        int windowHeight;
        std::string windowTitle;
        float cameraFov;
        float cameraNear;
        float cameraFar;
        float cameraSpeed;
        float cameraSensitivity;
        glm::vec4 clearColor;

        std::string skydomeTexture;
        std::string skydomeVertShader, skydomeFragShader;

    private:
        Config() {}
};