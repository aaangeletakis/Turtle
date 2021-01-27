#ifndef CMD_LINE_H
#define CMD_LINE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"

static void help(const char * progName){
    const char *whoami = strrchr(progName, '/');
    if (whoami)
        whoami++;
    else
        whoami = progName;
    printf(
                "usage: %s [option] ... [file] [arg] ...\n"
                "Translate python source code to C++ source code\n"
                "Options:\n"
                "-h     : print this help message and exit (also --help)\n"
                "-v     : verbose\n"
                "-V     : print the Python version number and exit (also --version)\n",

    whoami);
    exit(EXIT_SUCCESS);
}

#define __NUM_OF_ELEMENTS(ARR) (sizeof(ARR)/sizeof(ARR[0]))
unsigned int start(int argc, char * argv[], std::string& filename)
{
    if(argc == 1){
        help(argv[0]);
    }
    enum                        {HELP_ARG,                     VERSION_ARG,              VERBOSE_ARG, FILE_ARG};
    const char * args[] =       {"--help", "-h",     "-?",     "--version", "-V",        "-v",        "--file"};
    unsigned char arg_list[] =  {HELP_ARG, HELP_ARG, HELP_ARG, VERSION_ARG, VERSION_ARG, VERBOSE_ARG, FILE_ARG};
    unsigned int flags = 0;

    //skip prog name
        for(unsigned int arg = 1; arg < (unsigned int)argc; ++arg){
            for(unsigned int i = 0; i < (unsigned int)__NUM_OF_ELEMENTS(args); ++i){
                if(!strcmp(argv[arg], args[i])){
                    flags |= 1 << arg_list[i];
                    goto CMD_ARGUMENT_FOUND;
                }
            }


            //argument is not found / unkown arg
            filename = argv[arg];


            CMD_ARGUMENT_FOUND:;
        }
     return flags;
}
#undef __NUM_OF_ELEMENTS

#endif // CMD_LINE_H
