#ifndef TURTLE_COMMMAND_LINE_FUNCS_H
#define TURTLE_COMMMAND_LINE_FUNCS_H

#include "macros.h"
#include "stdio.h"

auto help()
{
    printf("Turtle.exe [filename]\n"
           "Translate python source code to C++ source code\n");
    return 0;
}

#endif // TURTLE_COMMMAND_LINE_FUNCS_H