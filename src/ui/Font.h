#pragma once

#include <CharInfo.h>
#include <FontParser.h>
#include <Texture.h>
#include <unordered_map>

class Font
{
    public:
        void load(std::string fntPath, Texture* texture);
        
        std::unordered_map<int, CharInfo> chars;
        Texture* texture;
        int lineHeight = 45;
};