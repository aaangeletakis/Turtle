#ifndef IO_H
#define IO_H

#include "global.h"
#include <fstream>
#include <streambuf>
#include <sstream>
#include <codecvt>

void readfile(const char* filename, std::string& data){
    //https://stackoverflow.com/a/2602060
    std::ifstream file(filename);

    file.seekg(0, std::ios::end);
    data.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    data.assign((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());

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


void writefile(const char* filename, std::string& data){
    std::ofstream file(filename);
    file << data;
    file.close();
}

#endif // IO_H
