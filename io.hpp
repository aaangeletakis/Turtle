#ifndef IO_H
#define IO_H

#include "global.h"
#include <fstream>
#include <string.h>
#include <exception>

void readfile(const char *filename, std::string &file)
{
    class file_exception: public std::exception
    {
      virtual const char* what() const throw()
      {
        return strerror(errno);
      }
    } fileex;

    FILE *fh = fopen(filename, "rb");
    if ( fh == NULL )
    {
        //error: No such file or directory

        //am i a joke to you?
        //I should have read in between the lines ...
        throw fileex;
    }
    fseek(fh, 0L, SEEK_END);
    const size_t& length = ftell(fh);
    /*it's*/ rewind(fh); //timeeee

    file.resize(length, 0);
    fread(file.data(), length, sizeof(char), fh);
    if(ferror(fh) || feof(fh))
    {
        throw fileex;
    }
    fclose(fh); fh = NULL;
}

void writefile(const char *filename, std::string &data)
{
    std::ofstream file(filename);
    file << data;
    file.close();
}

#endif // IO_H
