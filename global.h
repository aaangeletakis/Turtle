#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <stdio.h>

//#define DEBUG

#ifndef DEBUG
#define NDEBUG
#endif

#ifdef DEBUG
#define DEBUG_M(x) x
#define NDEBUG_M(x)

#else
#define DEBUG_M(x)
#define NDEBUG_M(x) x
#endif

auto print(const std::string &str)
{
    puts(str.c_str());
    return 0;
}

auto panic(const std::string &str)
{
    fprintf(stderr, "ERROR: %s\n", str.c_str());
    exit(1);
}

auto panic()
{
    exit(1);
}

#endif // GLOBAL_H
