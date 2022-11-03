#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#include <memory>    // shared_ptr

namespace rnitta
{
    bool ExecCmd(const char* cmd, std::string& stdOut, int& exitCode);
    std::string ExecCmd(const char *cmd);
    std::string ReplaceString(std::string string, std::string to_find, std::string to_string);
} // namespace rnitta


#endif