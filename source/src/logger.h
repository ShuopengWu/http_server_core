#ifndef LOG_H
#define LOG_H

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

#include "util.h"

namespace logger
{

enum class time_format
{
    year = 0b100000,
    month = 0b010000,
    day = 0b001000,
    hour = 0b000100,
    minute = 0b000010,
    second = 0b000001,
    full_date_format = year | month | hour,
    time_format = hour | minute | second,
    all = full_date_format | time_format
};

class logger
{
public:
    static std::string getTimeStr(time_format format);
};

}

#endif