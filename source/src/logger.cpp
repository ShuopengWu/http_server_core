#include "logger.h"

namespace logger
{

std::string logger::getTimeStr(time_format format)
{
    std::time_t time = util::getCurrentTime();
    std::string format_str;
    std::tm time_struct;

    if (localtime_r(&time, &time_struct) == nullptr)
        return "";

    switch (format)
    {
    case time_format::year:             format_str = "%Y";                   break;
    case time_format::month:            format_str = "%m";                   break;
    case time_format::day:              format_str = "%d";                   break;
    case time_format::hour:             format_str = "%H";                   break;
    case time_format::minute:           format_str = "%M";                   break;
    case time_format::second:           format_str = "%S";                   break;
    case time_format::full_date_format: format_str = "%m/%d/%Y";             break;
    case time_format::time_format:      format_str = "%H:%M:%S";             break;
    case time_format::all:              format_str = "%Y-%m-%d %H:%M:%S";    break;
    default: return "";
    }

    std::stringstream ss;
    ss << std::put_time(&time_struct, format_str.c_str());
    return ss.str();
}

}
