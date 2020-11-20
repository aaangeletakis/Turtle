#ifndef MAIN_H
#define MAIN_H
#include <regex>
#include "magic.hpp"
#include <iostream>

void help();
auto getPhysicalLineTokens(std::string& filedata){

    //([rR]?"""(?:[^\\"]|\\.)*"""|([rR]?'(?:[^\\']|\\.)*'|[rR]?"(?:[^\\"]|\\.)*")|(#[^\r\n]*)|(\n[ \t]*)|(\\[^\r\n]*)|^|(([<>*\/]{2}=?)|([=<>!+\-*\\&|]=))|([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+))
    std::regex TokenRegex(
                //Capture doc strings
                R"(([rR]?"""(?:[^\\"]|\\.)*""")"

                //Capture single ' and double " qoute strings
                R"(|([rR]?'(?:[^\\']|\\.)*'|[rR]?"(?:[^\\"]|\\.)*"))"

                //Capture python single comment
                R"(|(#[^\r\n]*))"

                //Capture newline and its whitespace
                R"(|(\n[ \t]*))"

                //Capture backslash '\' and anything that comes after it, so we can quickly ignore it
                R"(|(\\[^\r\n]*))"

                //Capture just newline if haven't in the previos step
                R"(|^)"

                //Capture 2 character operators
                R"(|(([<>*\/]{2}=?)|([=<>!+\-*\\&|]=)))"

                //capture anything else, symbols, and any other arithmitic
                R"(|([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+)))",

                std::regex_constants::optimize);

    std::sregex_token_iterator rend, rit(filedata.begin(), filedata.end(), TokenRegex);
    ++rit; // The first token is always "" so skip over it
    turtle::Node Document;
    Document.Nodes.reserve(std::distance(rit, rend));

    turtle::Node Node;
    for(uint32_t lineIndex = 0; rit != rend; ++rit){
        const auto& token = rit->str();
        switch(token[0]){
        /* ignore '\' */
        case '\\':{break;}
        case '\n': case '\r':{

            /*
             *    ┌──> Flag Type - Control Class Id
             *    │                           ┌──> Amount Of whitespace (11 bits -> max 2,048 characters)
             * ┌──┤                   ┌───────┴──┐┌───> Is newline
             * 00001111  11111111 11111111 11111111
             *     |                 |
             *     └─────────────────┴──> Line Number (16 bits -> max 65,536 lines)
             */

            // The regex matches the newline + whitespace at the beginning,
            // so subtract 1 and to get how much whitespace there is
            Node.NodeFlags = (++lineIndex << 12) | ((token.size() - 1) << 1) | turtle::token::flag::NEWLINE;
            Document.Nodes.push_back(Node);
            break;
        }
        default:{
            for(unsigned int i = 0; i < 65; ++i){
                if(token == turtle::turtleBuiltinTokenMap[i].TokenString){
                    Node.NodeFlags = turtle::turtleBuiltinTokenMap[i].TokenFlags;
                    break;
                }
            }

            if(Node.NodeFlags){
                Document.identifiers.push_back(token);
                Node.NodeFlags = turtle::token::flag::IDENTIFIER | (Document.identifiers.size() - 1);
            }
            Document.Nodes.push_back(Node);
            break;
        }
        }
    }
}

void start(int argc, char *argv[]){
    switch (argc)
       {
       case 1:
       {
          help();
          exit(EXIT_SUCCESS);
          break;
       }
       case 2:
       {
          if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
          {
             help();
             exit(EXIT_SUCCESS);
          }
          break;
       }
       default:
       {
          std::cerr << "To many args\n";
          exit(EXIT_SUCCESS);
          break;
       }
    }
}

void help()
{
   printf("Turtle.exe [filename]\n"
          "Translate python source code to C++ source code\n");
   exit(EXIT_SUCCESS);
}

#endif // MAIN_H
