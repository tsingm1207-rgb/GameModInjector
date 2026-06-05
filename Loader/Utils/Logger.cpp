#include  "Logger.h"

bool Logger::Init(const std::filesystem::path& logFilePath)
{
    logFile_.open(logFilePath, std::ios::out | std::ios::app);
    
    if(!logFile_.is_open())
    {
        return false;
    }
    initialized = true;
    return true;
}

void Logger::Shutdown()
{
    if(!initialized)
        return;
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
    localtime_s(&localTime, &time);
    logFile_ << "[" << std::put_time(&localTime, "%H:%M:%S") << "] [INFO] Logger关闭" << std::endl;
    logFile_.close();
}

void Logger::Log(uint32_t level, const std::string& msg)
{
    if (!initialized)
        return;

    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
    localtime_s(&localTime, &time);

    logFile_ << "["  << std::put_time(&localTime, "%H:%M:%S") << "] ["  << LevelToString(level) << "] " << msg << std::endl;
    logFile_.flush();
}

const char* Logger::LevelToString(uint32_t level)
{
    switch (level)
    {
    case 0:
        return "DEBUG";
    case 1:
        return "INFO";
    case 2:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}