#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <filesystem>

namespace util
{
static void error_if(bool condition, std::string error_message)
{
    if (condition)
    {
        
    }
}

static inline std::time_t getCurrentTime()
{
    std::chrono::time_point now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}

static std::string get_absoute_path(std::string filename)
{
    if (filename.empty())
        return "";
        
    try
    {
        std::filesystem::path path(filename);
        return path.is_absolute() ? filename : std::filesystem::absolute(path).string();
    }
    catch (const std::filesystem::filesystem_error &)
    {
        return "";
    }
}

}

#endif