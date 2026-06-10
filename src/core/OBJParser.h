#pragma once
#include <FileSystem.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>

class OBJParser
{
    public:
        static std::map<std::string, std::vector<float>> getVertices(std::string path);
};