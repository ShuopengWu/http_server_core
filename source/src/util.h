#ifndef UTIL_H
#define UTIL_H

#include <filesystem>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "logger.h"

#define DECLARE_PROPERTY(TYPE, NAME) \
private: \
    TYPE NAME; \
public: \
    inline TYPE get_##NAME() const { return NAME; } \
    inline void set_##NAME(TYPE other) { NAME = other; }

#define DECLARE_ONLY_READ_PROPERTY(TYPE, NAME) \
private: \
    TYPE NAME; \
public: \
    inline TYPE get_##NAME() const { return NAME; }

namespace util
{
static void error_assert(bool condition, std::string error_message, bool is_need_exit = true)
{
    if (!condition)
    {
        logger::logger &log = logger::logger::instance();
        log.show_error_log(error_message);
        if (is_need_exit)
            exit(EXIT_FAILURE);
    }
}

static void error_assert_with_errno(bool condition, std::string error_message, bool is_need_exit = true)
{
    if (!condition)
    {
        logger::logger &log = logger::logger::instance();
        log.show_error_log(error_message + ", errno state is : " + std::strerror(errno));
        if (is_need_exit)
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

static void close_fd(int &fd)
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

}

#endif