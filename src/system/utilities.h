#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <thread>

namespace AO
{
    std::string AO_StrFormat(const char* str, ...);

    size_t ThreadID();

}

#endif // UTILITIES_H
