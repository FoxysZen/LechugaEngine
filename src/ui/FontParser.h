#pragma once

#include <CharInfo.h>
#include <FileSystem.h>
#include <string>
#include <unordered_map>

class FontParser {
public:
    static std::unordered_map<int, CharInfo> parse(std::string path);
};