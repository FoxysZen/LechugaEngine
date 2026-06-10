#include <Logger.h>

void Logger::info(std::string msg)
{
    std::cout << "[Info] " << msg << std::endl;
}

void Logger::warn(std::string msg)
{
    std::cout << "[Warning] " << msg << std::endl;
}

void Logger::error(std::string msg)
{
    std::cout << "[Error] " << msg << std::endl;
}
