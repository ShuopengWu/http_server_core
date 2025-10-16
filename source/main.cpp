#include <iostream>

#include "src/logger.h"

int main()
{
    logger::logger l;
    std::cout << l.getTimeStr(logger::time_format::year) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::month) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::day) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::hour) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::minute) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::second) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::full_date_format) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::time_format) << std::endl;
    std::cout << l.getTimeStr(logger::time_format::all) << std::endl;
    return 0;
}