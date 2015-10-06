#pragma once

#include <iostream>

namespace hmk
{
class Debug
{
public:
    template<typename T>
    static void print(T t)
    {
        std::cout << t;
    }

    template<typename T, typename... TArgs>
    static void print(T t, TArgs... targs)
    {
        print(t);
        print(targs...);
    }
private:
    Debug() {}
    Debug(const Debug&);
    Debug& operator=(const Debug&);
};
}
