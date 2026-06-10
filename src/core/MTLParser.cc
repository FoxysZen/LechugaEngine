#include <MTLParser.h>

std::map<std::string, std::string> MTLParser::getTexture(std::string path)
{
    std::string file = FileSystem::readFile(path);
    std::istringstream stream(file);
    std::string line;
    std::map<std::string, std::string> textures;

    std::string currentMaterial = "";
    while (std::getline(stream, line))
    {
        if (line.substr(0, 7) == "newmtl ")
        {
            currentMaterial = line.substr(7);
        }
        else if (line.substr(0, 7) == "map_Kd ")
        {
            textures[currentMaterial] = line.substr(7);
        }
    }

    return textures;
}