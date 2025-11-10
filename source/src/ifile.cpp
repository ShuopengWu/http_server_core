#include "ifile.h"

#include "logger.h"

namespace util
{

IFile::IFile(const std::string &filename)
{
    this->filename = util::get_absoute_path(filename);
}

IFile::~IFile()
{
}

std::pair<bool, std::string> IFile::read_for_file()
{
    logger::logger &logger = logger::logger::instance();

    if (!is_file_exist()) 
    {
        logger.show_error_log("Read for file error : file not exist.");
        return std::pair<bool, std::string>(false, "");
    }

    std::ifstream ifs(filename, std::ios::in /* 输入模式 */ | std::ios::binary /* 二进制模式 */);
    if (!ifs.is_open())
    {
        logger.show_error_log("Read for file error : file can not open.");
        return std::pair<bool, std::string>(false, "");
    }

    try
    {
        // 高效读取整个文件内容
        std::string file_content = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
        logger.show_info_log("Read for file " + filename + " bytes : " + std::to_string(file_content.size()));
        return std::pair<bool, std::string>(true, std::move(file_content));
    }
    catch (std::exception &e)
    {
        logger.show_error_log("Read for file " + filename + " error : " + e.what());
        return std::pair<bool, std::string>(false, "");
    }
}

bool IFile::write_to_file(const std::string &s)
{
    logger::logger &logger = logger::logger::instance();

    if (!is_file_exist() && !create_file())
    {
        logger.show_error_log("Write to file " + filename + " error : file can not exist and can not create file.");
        return false;
    }
    
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);

    if (!ofs.is_open())
    {
        logger.show_error_log("Write to file error : file can not open.");
        return false;
    }

    std::ostreambuf_iterator<char> sit(ofs);
    std::copy(s.begin(), s.end(), sit);
    
    if (ofs.good())
    {
        logger.show_info_log("Successfully wrote " + std::to_string(s.size()) + " bytes to file: " + filename);
        return true;
    }
    else
    {
        logger.show_error_log("Write error: file '" + filename + "' failed to write all data (I/O error).");
        return false;
    }
}

void IFile::set_file_name(const std::string &filename)
{
    logger::logger &logger = logger::logger::instance();

    this->filename = util::get_absoute_path(filename);
    logger.show_info_log("File name updated to: " + this->filename);
}

bool IFile::is_file_exist()
{
    if (filename.empty())
        return false;
    
    fs::path file_path(filename);
    return fs::exists(file_path) && fs::is_regular_file(file_path);
}

bool IFile::create_file()
{
    logger::logger &logger = logger::logger::instance();
    
    if (filename.empty())
    {
        logger.show_error_log("Can not create file " + filename + ", filename is empty.");
        return false;
    }

    if (is_file_exist())
    {
        logger.show_warning_log("Can not create file " + filename + " because file is already existed.");
        return true;
    }

    try
    {
        fs::path file_path(filename);
        fs::path parent_path(file_path.parent_path());
        if (!parent_path.empty() && !fs::exists(parent_path))
        {
            if (!fs::create_directories(parent_path))
            {
                logger.show_error_log("Failed to create parent directories: " + parent_path.string());
                return false;
            }
        }

        std::ofstream ofs(filename, std::ios::out);
        if (ofs.is_open()) {
            ofs.close();
            logger.show_info_log("File created successfully: " + filename);
            return true;
        }

        logger.show_error_log("Failed to open file for creation: " + filename);
        return false;
    }
    catch(const fs::filesystem_error& e)
    {
        logger.show_error_log("Filesystem error during create_file: " + std::string(e.what()));
        return false;
    }
}

std::string IFile::current_file_name()
{
    if (is_file_exist())
        return filename;
    return "";
}



} // namespace util
