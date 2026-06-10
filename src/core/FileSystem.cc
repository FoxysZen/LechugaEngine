#include <FileSystem.h>

std::string FileSystem::readFile(std::string path)
{
    std::ifstream file;
    file.open(path, std::ifstream::in);

    if (!file.is_open())
    {
        Logger::error("FileSystem: " + path + " could not be opened");
        return "";
    }

    std::string content = "", s;
    while (getline(file, s))
    {
        content += s + '\n';
    }

    return content;
}