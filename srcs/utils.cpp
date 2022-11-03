#include "utils.hpp"

namespace rnitta
{

    bool ExecCmd(const char *cmd, std::string &stdOut, int &exitCode)
    {
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE *p)
                                   { exitCode = pclose(p); });
        if (!pipe)
        {
            return false;
        }
        std::array<char, 256> buf;
        while (!feof(pipe.get()))
        {
            if (fgets(buf.data(), buf.size(), pipe.get()) != nullptr)
            {
                stdOut += buf.data();
            }
        }
        return true;
    }

    std::string ExecCmd(const char *cmd)
    {
        std::string stdOut;
        int exitCode;
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE *p)
                                   { exitCode = pclose(p); });
        if (!pipe)
        {
            return ("");
        }
        std::array<char, 256> buf;
        while (!feof(pipe.get()))
        {
            if (fgets(buf.data(), buf.size(), pipe.get()) != nullptr)
            {
                stdOut += buf.data();
            }
        }
        return (stdOut);
    }

}