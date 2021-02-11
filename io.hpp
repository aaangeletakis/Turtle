#ifndef IO_H
#define IO_H

#include "global.h"
#include <fstream>
#include <streambuf>
#include <sstream>
#include <codecvt>

void readfile(const char *filename, char * buffer)
{
    FILE *fh = fopen(filename, "rb");
    if ( fh == NULL )
    {
        //am i a joke to you?
        //I should have read in between the lines ...
        panic("Cannot open file\n");
    }
    fseek(fh, 0L, SEEK_END);
    const size_t length = ftell(fh);
    /*it's*/ rewind(fh); //timeeee
    //avoid warning -- put in if statment
    if(fread(buffer, length, sizeof(char), fh)){};
    if(ferror(fh))
    {
        panic("Error reading file\n");
    }
    if(feof(fh))
    {
        panic("End of file reached while reading\n");
    }
    fclose(fh); fh = NULL;
}

/*void readFile(const char* filename, std::wstring& data)
{
    std::wifstream wfile(filename);
    std::locale::global(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));

    wfile.seekg(0, std::ios::end);
    data.reserve(wfile.tellg());
    wfile.seekg(0, std::ios::beg);

    data.assign((std::istreambuf_iterator<wchar_t>(wfile)),
                 std::istreambuf_iterator<wchar_t>());
}*/

void writefile(const char *filename, std::string &data)
{
    std::ofstream file(filename);
    file << data;
    file.close();
}

#endif // IO_H
