#include <FontParser.h>

std::unordered_map<int, CharInfo> FontParser::parse(std::string path)
{
    std::unordered_map<int, CharInfo> chars;
    std::string file = FileSystem::readFile(path);
    std::istringstream stream(file);
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.substr(0, 4) != "char") continue;
        if (line.substr(0, 5) == "chars") continue;

        CharInfo info;
        int id;

        auto getValue = [&](std::string key) -> int {
            int pos = line.find(key + "=");
            if (pos == std::string::npos) return 0;
            pos += key.size() + 1;
            return std::stoi(line.substr(pos));
        };

        id = getValue("id");
        info.x = getValue("x");
        info.y = getValue("y");
        info.width = getValue("width");
        info.height = getValue("height");
        info.xoffset = getValue("xoffset");
        info.yoffset = getValue("yoffset");
        info.xadvance = getValue("xadvance");

        chars[id] = info;
    }
    return chars;
}