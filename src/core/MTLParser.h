#pragma once
#include <FileSystem.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>

class MTLParser
{
    public:
        static std::map<std::string, std::string> getTexture(std::string path);
};