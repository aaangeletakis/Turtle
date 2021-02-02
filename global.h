#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define DEBUG 1

#define DEBUG_CPP (DEBUG && __cplusplus)

#if !DEBUG
#define NDEBUG
#endif

#if DEBUG
#define DEBUG_M(x) x
#define NDEBUG_M(x)

#else
#define DEBUG_M(x)
#define NDEBUG_M(x) x
#endif

//if C
#ifndef __cplusplus
#define constexpr const
#endif

void panic(const char *fmt, ...){
    fprintf(stderr, "error: ");

    va_list arglist;
    va_start( arglist, fmt );
    vfprintf(stderr, fmt, arglist);
    va_end( arglist );

    exit(EXIT_FAILURE);
}

void panic()
{
    exit(EXIT_FAILURE);
}

#endif // GLOBAL_H
