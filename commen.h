#ifndef COMMEN_H
#define COMMEN_H
#include <global.h>

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

#endif // COMMEN_H
