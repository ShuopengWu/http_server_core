#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <cerrno>
#include <cstring>
#include "logger.h"

namespace util
{
static void error_assert(bool condition, std::string error_message)
{
    if (!condition)
    {
        logger::logger &log = logger::logger::instance();
        log.show_error_log(error_message);
        exit(EXIT_FAILURE);
    }
}

static void error_assert_with_errno(bool condition, std::string error_message)
{
    if (!condition)
    {
        logger::logger &log = logger::logger::instance();
        log.show_error_log(error_message + ", errno state is : " + std::strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static std::string get_absoute_path(const std::string &filename)
{
    if (filename.empty())
        return "";
        
    try
    {
        std::filesystem::path path(filename);
        return path.is_absolute() ? filename : std::filesystem::absolute(path).string();
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        logger::logger &log = logger::logger::instance();
        log.show_error_log("Fail to get absoute path : " + std::string(e.what()));
        return "";
    }
}

}

#endif