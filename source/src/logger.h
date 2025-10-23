#ifndef LOG_H
#define LOG_H

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>
#include <mutex>
#include <chrono>

namespace
{
static std::string LOCAL_RED    =   "\033[31;1m";
static std::string LOCAL_YELLOW	=   "\033[0;33m";
static std::string LOCAL_GREEN	=	"\033[0;32m";
static std::string LOCAL_BULE	=	"\033[0;34m";
static std::string LOCAL_BLACK  =	"\033[0m";

static constexpr size_t MAX_TIME_STR_LEN    =   15;
static constexpr size_t MAX_LEVEL_STR_LEN   =   9;
}

namespace logger
{

enum class logger_level
{
    error,
    warning,
    info,
    debug,
    normal
};


class logger
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

public:
    static logger &instance();
    void show_error_log(std::string s, bool is_need_align = true);
    void show_waring_log(std::string s, bool is_need_align = true);
    void show_info_log(std::string s, bool is_need_align = true);
    void show_debug_log(std::string s, bool is_need_align = true);
    void set_min_logger_level(logger_level l);
    logger(const logger &) = delete;
    logger(const logger &&) = delete;
    void operator = (const logger &) = delete;
    void operator = (const logger &&) = delete;
private:
    logger();
    void show_log(const std::string &s, logger_level l, bool is_need_align);
    void align_format(std::string &s, int align_size);
    std::string get_time_str(time_format format);
    std::string get_color_str(logger_level l);
    std::string get_level_str(logger_level l);
    std::time_t getCurrentTime();

private:
    std::mutex show_log_mutex;
    logger_level min_logger_level;
    bool is_need_save_to_file;
};

}

#endif