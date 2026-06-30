#include <Font.h>

void Font::load(std::string fntPath, Texture *texture)
{
    this->texture = texture;
    chars = FontParser::parse(fntPath);

    std::string file = FileSystem::readFile(fntPath);
    std::istringstream stream(file);
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.substr(0, 6) == "common")
        {
            int pos = line.find("lineHeight=") + 11;
            lineHeight = std::stoi(line.substr(pos));
            break;
        }
    }
}