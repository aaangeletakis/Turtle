#include <iostream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <regex>
//#define NDEBUG
#include "json.hpp"
#include "stdio.h"

bool is_Newline(const std::string &tmp) { return tmp[0] == '\n'; }

auto help()
{
   printf("Turtle.exe [filename]\n"
          "Translate python source code to C++ source code\n");
   return 0;
}

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

bool writeToFile(std::string filename, std::string data)
{
   std::ofstream file(filename);
   file.write(data.c_str(), data.length());
   file.close();
   return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
   //only accept one argument
   switch (argc)
   {
   case 1:
   {
      help();
      return EXIT_SUCCESS;
      break;
   }
   case 2:
   {
      if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
      {
         help();
         return EXIT_SUCCESS;
      }
      break;
   }
   default:
   {
      std::cerr << "To many args\n";
      return EXIT_FAILURE;
      break;
   }
   }

   const char *filename = argv[1];

   //incase I ever want to do more than one file at once in the future
   std::vector<const char *> files;
   files.push_back(filename);
   //std::for each with the return as a break statment
   (void)std::any_of(files.begin(), files.end(), [&](auto &file) {
      {
         std::string filedata = "";

         if (getFileDataReadOnly(file, filedata))
         {
            std::cerr << "\033[1;31mError:\033[0m Could not open file '" << file << "'\n";
            return EXIT_FAILURE;
         }

         if (filedata.empty())
         {
            std::cerr << "\033[1;31mError:\033[0m file " << file << "is empty\n";
            return EXIT_FAILURE;
         }
         std::regex TokenRegex(R"(("""(?:[^\\"]|\\.)*"""|('(?:[^\\']|\\.)*'|"(?:[^\\"]|\\.)*")|(#[^\n]*)|(\n[ \t]*)|^|(([<>*\/]{2}=?)|([=<>!+\-*\\&|]=))|([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\t\f\v\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+)))");
         std::regex_iterator<std::string::iterator> rend, rit(filedata.begin(), filedata.end(), TokenRegex);
         ++rit; // The first token is always "" so skip over it
         // .reserve(std::distance(rit, rend));
         nlohmann::json DocumentJSON;
         DocumentJSON["DocumentData"] = R"({"lang": "Python"})"_json;
         DocumentJSON["tokens"] = nlohmann::json::array();
         auto &DocumentTokens = DocumentJSON["tokens"];
         for (unsigned int lineIndex = 0, tokenIndex = 0, scope = 0;
              rit != rend;
              ++rit)
         {
            const std::string tok = rit->str();
            if (tok[0] == '\n' || tok[0] == '\r')
            {
               ++lineIndex;
               scope = tok.size() - 1; // the number of spaces minis the '\n' character
            }
            else
            {
               ++tokenIndex;
               DocumentTokens.push_back(std::move(nlohmann::json::array({lineIndex, scope, tokenIndex, std::move(tok)})));
            }
         }
         writeToFile(std::string(file) + ".json", DocumentJSON.dump());
      }

      /*std::cout << "Line Number, Number of Whitespace tokens on its current line, TokenNumber, token String\n";
      */

      return EXIT_SUCCESS;
   });
   return EXIT_SUCCESS;
}
