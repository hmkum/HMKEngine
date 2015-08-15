#pragma once

#include <iostream>

namespace hmk
{
class Debug
{
public:
    template<typename T>
    static void Print(T t)
    {
        std::cout << t;
    }

    template<typename T, typename... TArgs>
    static void Print(T t, TArgs... targs)
    {
        Print(t);
        Print(targs...);
    }
private:
    Debug() {}
    Debug(const Debug&);
    Debug& operator=(const Debug&);
};
}
