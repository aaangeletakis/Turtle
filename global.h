#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <stdio.h>
#include <stdarg.h>

#define DEBUG

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

void panic(const char *fmt, ...){
    fprintf(stderr, "error: ");

    va_list arglist;
    va_start( arglist, fmt );
    vfprintf(stderr, fmt, arglist);
    va_end( arglist );

    exit(EXIT_FAILURE);
}

void panic(std::string fmt, ...){
    va_list arglist;
    va_start( arglist, fmt );
    panic(fmt.c_str(), arglist);
    va_end( arglist );
}

void panic()
{
    exit(EXIT_FAILURE);
}

#endif // GLOBAL_H
