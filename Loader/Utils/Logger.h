#pragma once
#include <filesystem>
#include <fstream>
class Logger
{
public:
    bool Init(const std::filesystem::path& logFilePath);
    void Shutdown();

    void Log(uint32_t level, const std::string& msg);

private:
    const char* LevelToString(uint32_t level);

    std::ofstream logFile_;
    bool initialized = false;
};