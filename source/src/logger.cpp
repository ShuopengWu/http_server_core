#include "logger.h"

namespace logger
{

logger::logger() :
    min_logger_level(logger_level::normal),
    is_need_save_to_file(false)
{

}

std::string logger::get_time_str(time_format format)
{
    std::time_t time = getCurrentTime();
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

logger &logger::instance()
{
    static logger l;
    return l;
}

void logger::show_error_log(std::string s, bool is_need_align)
{
    show_log(s, logger_level::error, is_need_align);
}

void logger::show_waring_log(std::string s, bool is_need_align)
{
    show_log(s, logger_level::warning, is_need_align);
}

void logger::show_info_log(std::string s, bool is_need_align)
{
    show_log(s, logger_level::info, is_need_align);
}

void logger::show_debug_log(std::string s, bool is_need_align)
{
    show_log(s, logger_level::debug, is_need_align);
}

void logger::set_min_logger_level(logger_level l)
{
    min_logger_level = l;
}

void logger::show_log(const std::string &s, logger_level l, bool is_need_align)
{
    if (static_cast<int>(l) > static_cast<int>(min_logger_level))
        return;

    std::stringstream ss;

    auto add_log = [&ss, this, is_need_align](std::string s, int align_size){
        if (is_need_align)
            align_format(s, align_size);
        ss << s;
    };

    add_log(get_level_str(l), MAX_LEVEL_STR_LEN);
    add_log(get_time_str(time_format::all), MAX_TIME_STR_LEN);
    ss << " ";
 
    if (s.size() >= 1 && s.back() == '\n')
    {
        std::string str = s;
        str.resize(str.size() - 1);

        if (str.size() >= 1 && str.back() == '\r')
            str.resize(str.size() - 1);
        
        if (str.size() >= 1 && str.back() != '.')
            str.push_back('.');
        ss << str;
    }
    else {
        if (s.size() >= 1 && s.back() != '.')
        {
            std::string str = s;
            str.push_back('.');
            ss << str;
        }
        else
            ss << s;
    }

    {
        std::unique_lock<std::mutex> lock(show_log_mutex);
        std::cout << get_color_str(l) << ss.str() << get_color_str(logger_level::normal) << std::endl;
    }
}

void logger::align_format(std::string &s, int align_size)
{
    if (s.size() < align_size)
        s.resize(align_size, ' ');
}

std::string logger::get_color_str(logger_level l)
{
    switch (l)
    {
        case logger_level::debug:   return LOCAL_BULE;
        case logger_level::error:   return LOCAL_RED;
        case logger_level::info:    return LOCAL_GREEN;
        case logger_level::warning:  return LOCAL_YELLOW;
        case logger_level::normal:
        default: return LOCAL_BLACK;
    }
}
std::string logger::get_level_str(logger_level l)
{
    switch (l)
    {
        case logger_level::debug:   return "[DEBUG]";
        case logger_level::error:   return "[ERROR]";
        case logger_level::info:    return "[INFO]";
        case logger_level::warning:  return "[WARNING]";
        case logger_level::normal:
        default: return "[NORMAL]";
    }
}
std::time_t logger::getCurrentTime()
{
    std::chrono::time_point now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}

}
