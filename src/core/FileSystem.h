#pragma once
#include <fstream>
#include <Logger.h>
#include <sstream>
#include <string>

class FileSystem
{
    public:
        static std::string readFile(std::string path);
};