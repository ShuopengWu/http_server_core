#ifndef IFILE_H
#define IFILE_H

#include <string>
#include <fstream>
#include <filesystem>

#include "logger.h"

namespace util
{

namespace fs = std::filesystem;

class IFile
{
public:
    IFile(const std::string &filename = "");
    ~IFile();

    std::pair<bool, std::string> read_for_file();
    bool write_to_file(const std::string &s);
    void set_file_name(const std::string &filename);
    bool is_file_exist();
    bool create_file();
    std::string current_file_name();
private:
    std::string filename;
};


} // namespace util

#endif
