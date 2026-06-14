#include <Font.h>

void Font::load(std::string fntPath, Texture* texture)
{
    this->texture = texture;
    chars = FontParser::parse(fntPath);
}