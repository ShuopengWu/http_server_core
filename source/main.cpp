#include <iostream>

#include "src/ifile.h"

int main()
{
    logger::logger &log = logger::logger::instance();
    log.set_min_logger_level(logger::logger_level::normal);
    util::IFile f;
    f.set_file_name("1.log");
    f.write_to_file("11111");
    auto s = f.read_for_file();
    std::cout << s.first << s.second;
}
