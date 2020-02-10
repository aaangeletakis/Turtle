#include <iostream>
#include <algorithm>
#include <regex>
#include <bitset>
#include "turtle.h"
#include "token.h"
//#define NDEBUG
#include "json.hpp"

bool is_Newline(const std::string &tmp) { return tmp[0] == '\n'; }
/*struct DocRoot : std::vector<DocNode>
{
   unsigned int numberOfTokens = 0;
   unsigned int scopeIndex = 0;
   unsigned int lineIndex = 0;
   void create_child(const std::string &tokenData, const unsigned int NodeType)
   {
      if (is_Newline(tokenData))
      {
         ++lineIndex;
         scopeIndex += (tokenData.length() - 1) - scopeIndex;
         //scopeIndex;
      }
      push_back(std::move(DocNode(tokenData, NodeType, numberOfTokens, scopeIndex)));
      ++numberOfTokens;
   }
};*/

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
   std::vector<const char *> files(1);
   files.push_back(filename);
   //std::for each with the return as a break statment
   nlohmann::json DocumentJSON;
   DocumentJSON["DocumentData"] = R"({"lang": "C++"})"_json;
   DocumentJSON["tokens"] = nlohmann::json::array();

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
         // .reserve(std::distance(rit, rend));
         for (unsigned int lineIndex = 0, tokenIndex = 0, scope = 0, oldScope = 0, scopeIndex = 0;
              rit != rend;
              ++rit)
         {
            const std::string tok = rit->str();
            if (tok[0] == '\n' || tok[0] == '\r')
            {
               ++lineIndex;
               oldScope = std::move(scope);
               scope = tok.size() - 1; // the number of spaces minis the '\n' character
               if (scope >= oldScope)
               {
                  ++scopeIndex;
               }
               else
               {
                  scopeIndex = 0;
               }
            }
            else
            {
               ++tokenIndex;
               for (int type = 0; type < TURTLE_ENUM_NUMBER_OF_TOKEN_TYPES; ++i)
               {
                  try {

                  } catch(std::out_of_range &e){}
               }
            }
            DocumentJSON["tokens"].push_back(nlohmann::json::array({lineIndex, scope, scopeIndex, tokenIndex, tok}));
         }
      }
      std::cout << DocumentJSON["tokens"].dump();
      //std::for_each(DocumentJSON["tokens"].begin(), DocumentJSON["tokens"].end(), [&]() {});
      return EXIT_SUCCESS;
   });
   //root.create_child(std::string("asdadas"), 1);
   //std::cout << root[0].TokenID;
   //root.make_child(&node2);
   //node2.NodeType = 8;
   //std::cout << root[0]->size;
   puts("\nDone");
   //std::vector<std::array<char, 2>> tokVecLabels(tokVec.size(), {0});
   return EXIT_SUCCESS;
}