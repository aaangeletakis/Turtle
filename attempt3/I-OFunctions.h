#ifndef TURTLE_I_O_FUNCTIONS_H
#define TURTLE_I_O_FUNCTIONS_H
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <string.h>

template <class T>
bool getFileDataReadOnly(const T &filename, std::string &fileDataString)
{
    std::ifstream tmp(filename);
    if (tmp.is_open())
    {
        tmp.seekg(0, std::ios::end);
        fileDataString.reserve(tmp.tellg());
        tmp.seekg(0, std::ios::beg);
        fileDataString.assign((std::istreambuf_iterator<char>(tmp)),
                              std::istreambuf_iterator<char>());
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

bool writeToFile(std::string &filename, std::string &data)
{
    std::ofstream file(filename);
    file.write(data.c_str(), data.length());
    file.close();
    return EXIT_SUCCESS;
}

#endif
