#pragma once

#include <string>
#include <fstream>

namespace hmk
{
class Logger
{
public:
    static Logger& get_instance()
    {
        static Logger instance;
        return instance;
    }

    bool initialize(std::string filename)
    {
        log_file_.open(filename, std::ios_base::out);
        return log_file_.is_open();
    }

    void shutdown()
    {
        if(log_file_.is_open())
        {
            log_file_.close();
        }
    }

    template<typename T>
    void write(T t)
    {
        log_file_ << t;
    }

    template<typename T, typename... TArgs>
    void write(T t, TArgs... targs)
    {
        write(t);
        write(targs...);
    }

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
    std::fstream log_file_;
private:
    Logger() {}
};
}
