#pragma once

#include <string>
#include <fstream>

namespace hmk
{
class Logger
{
public:
    static Logger& Inst()
    {
        static Logger instance;
        return instance;
    }

    bool Initialize(std::string filename)
    {
        mLogFile.open(filename, std::ios_base::out);
        return mLogFile.is_open();
    }

    void Shutdown()
    {
        if(mLogFile.is_open())
        {
            mLogFile.close();
        }
    }

    template<typename T>
    void Write(T t)
    {
        mLogFile << t;
    }

    template<typename T, typename... TArgs>
    void Write(T t, TArgs... targs)
    {
        Write(t);
        Write(targs...);
    }

private:
    std::fstream mLogFile;
private:
    Logger() {}
    Logger(const Logger&);
    Logger& operator=(const Logger&);
};
}
