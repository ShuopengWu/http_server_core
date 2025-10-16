#ifndef UTIL_H
#define UTIL_H

#include <chrono>

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

}

#endif