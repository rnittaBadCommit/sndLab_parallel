#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#include <memory>    // shared_ptr

namespace rnitta
{
    bool ExecCmd(const char* cmd, std::string& stdOut, int& exitCode);
} // namespace rnitta


#endif