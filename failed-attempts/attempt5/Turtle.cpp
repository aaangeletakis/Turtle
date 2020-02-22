#include <iostream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <regex>
#include "token.h"

#define DEBUG

#ifdef DEBUG
#include <chrono>
std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
#endif

#define printError(msg)                                       \
   {                                                          \
      std::cerr << "\033[1;31mError:\033[0m " << msg << '\n'; \
      std::exit(1);                                           \
   }

auto help()
{
   printf("Turtle.exe [filename]\n"
          "Translate python source code to C++ source code\n");
   return 0;
}

template <class T>
auto getFileDataReadOnly(const T &filename, std::string &fileDataString)
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
#ifdef DEBUG
   start = std::chrono::high_resolution_clock::now();
#endif
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

   //std::for_each with the return as a break statment
   (void)std::any_of(files.begin(), files.end(), [&](const auto &file) {
      {
         std::string filedata = "";
         std::ifstream tmp(file);

         if (getFileDataReadOnly(file, filedata))
         {
            printError("Could not open file '" << file << '\'');
            return EXIT_FAILURE;
         }

         if (filedata.empty())
         {
            printError("File " << file << " is empty");
            return EXIT_FAILURE;
         }
         std::regex TokenRegex(R"(([rR]?"""(?:[^\\"]|\\.)*"""|([rR]?'(?:[^\\']|\\.)*'|[rR]?"(?:[^\\"]|\\.)*")|(#[^\n]*)|(\n[ \t]*)|^|(([<>*\/]{2}=?)|([=<>!+\-*\\&|]=))|([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\t\f\v\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+)))", std::regex_constants::optimize);
         std::sregex_token_iterator rend, rit(filedata.begin(), filedata.end(), TokenRegex);
         ++rit; // The first token is always "" so skip over it

         std::vector<turtle::TokenDataStructure> Document;
         Document.reserve(std::distance(rit, rend));

         {
            unsigned int lineIndex = 0, tokenIndex = 0, scope = 0;
            std::for_each(rit, rend, [&](const auto &match) {
               const std::string &tok = match.str();
               const auto &startChar = tok[0];
               if (startChar == '\n' || startChar == '\r')
               {
                  ++lineIndex;
                  scope = tok.size() - 1; // the number of spaces/tabs minis the '\n' character
               }
               else
               {
                  ++tokenIndex;
                  Document.push_back({lineIndex, scope, tokenIndex, std::move(tok)});
               }
            });
         }

         filedata.clear();
         Document.shrink_to_fit();

         //writeToFile(std::string(file) + ".json", DocumentJSON);
         std::for_each(Document.begin(), Document.end(), [&](auto &Token) {
            switch (Token.TokenString[0])
            {
            case '"':
            case '\'':
               if (Token.TokenString.length() == 1)
               {
                  printError("Line " << Token.LineIndex + 1 << ": missing terminating " << Token.TokenString << " character");
               }
               Token.TokenFlags |= turtle::token::flag::DATA_TYPE_STRING;
               break;
            case 'r':
            case 'R':
            {
               const char &c = Token.TokenString[1];
               if (c == '\'' || c == '"')
               {
                  //The user/programmer can mess up and not properly close their string -> "Hello World
                  if (Token.TokenString.length() == 2)
                  {
                     printError("Line " << Token.LineIndex + 1 << ": missing terminating " << Token.TokenString << " character");
                  }
                  else
                  {
                     Token.TokenFlags |= turtle::token::flag::DATA_TYPE_RAW_STRING;
                  }
               }
               else
               {
                  Token.TokenFlags |= turtle::token::flag::IDENTIFIER;
               }
            }
            break;
            case '#':
               Token.TokenFlags |= turtle::token::flag::DATA_TYPE_COMMENT;
               break;
            case '(':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_CURVED_LEFT_BRACE;
               break;
            case ')':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_CURVED_RIGHT_BRACE;
               break;
            case '{':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_CURLY_LEFT_BRACE;
               break;
            case '}':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_CURLY_RIGHT_BRACE;
               break;
            case '[':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_SQUARE_LEFT_BRACE;
               break;
            case ']':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_SQUARE_RIGHT_BRACE;
               break;
            case ',':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_COMMA;
               break;
            case '.':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_ACCESS;
               break;
            case '@':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_AT_SIGN;
               break;
            case ';':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_SEMICOLON;
               break;
            case ':':
               Token.TokenFlags |= turtle::token::flag::DELIMITER_COLON;
               break;
            default:
            {
               auto it = std::find_if(std::begin(turtle::turtleBuiltinTokenMap), std::end(turtle::turtleBuiltinTokenMap), [&](const auto &builtinToken) {
                  return (Token.TokenString == builtinToken.TokenString);
               });
               if (it != std::end(turtle::turtleBuiltinTokenMap))
               {
                  Token.TokenFlags |= it->TokenFlags;
               }
               else
               {
                  Token.TokenFlags |= turtle::token::flag::IDENTIFIER;
               }
            }
            break;
            }
            //std::cout << Token.TokenString << " " << Token.TokenType << " " << Token.TokenFlags << '\n';
         });

//Print Out what happened in order to visually see if the operation succeeded
#ifdef DEBUG
         for (auto &t : Document)
         {
            std::bitset<sizeof(t.TokenFlags) * 8> bits(t.TokenFlags);

            std::cout << bits
                      << ' '
                      << (bits[bits.size() - 1]
                              ? "TURTLE_TOKEN_IDENTIFIER"
                              : (bits[bits.size() - 2] ? "TURTLE_TOKEN_DELIMITER " : (bits[bits.size() - 3] ? "TURTLE_TOKEN_ARITHMETIC" : (bits[bits.size() - 4] ? "TURTLE_TOKEN_KEYWORD   " : "TURTLE_TOKEN_DATA      "))))
                      << " \"" << t.TokenString << '"' << '\n';
         }
#endif
         //1) First translate lines into physical lines
         std::vector<std::vector<turtle::NodePair>> DocumentLines(1);
         {
            std::vector<turtle::NodePair> *currentScope = &DocumentLines[0];

            //First tokens line number
            unsigned int currentLine = Document[0].LineIndex;

            for (unsigned int i = 0; i < Document.size(); ++i)
            {
               auto &token = Document[i];
               if (currentLine != token.LineIndex)
               {
                  currentLine = token.LineIndex;
                  DocumentLines.push_back(std::vector<turtle::NodePair>());
                  currentScope = &DocumentLines.back();
                  currentScope->reserve(1);
               }
               currentScope->push_back({0, &token});
            }
         }
#ifdef DEBUG
         puts("\nPhysical lines");
         for (auto &s : DocumentLines)
         {
            std::cout << "Line: ";
            for (auto &t : s)
            {
               std::cout << '"' << ((turtle::TokenDataStructure *)t.NodePtr)->TokenString << "\" ";
            }
            std::cout << '\n';
         }
#endif
         //1) Begin parsing into logical lines

         //Search for line continuation character
         for (auto line = DocumentLines.begin(); line != DocumentLines.end(); ++line)
         {
            //it works
         GOTO_CHECK_FOR_LINE_CON_CHAR:
            const auto &pos = std::find_if(line->cbegin(), line->cend(), [&](const auto &NodePairToken) {
               const char &c = ((turtle::TokenDataStructure *)NodePairToken.NodePtr)->TokenString[0];
               if (c == '\\')
               {
                  return true;
               }
               return false;
            });

            //if '\' found
            if (pos != line->cend())
            {
               //If '\' is the last token on the line
               if (pos == line->cend() - 1)
               {
                  auto nextLine = (line + 1);

                  //if there is a '\' token but we are at the last line
                  if (nextLine == DocumentLines.end())
                  {
                     const auto &Token = (*(turtle::TokenDataStructure *)pos->NodePtr);
                     printError("Line " << Token.LineIndex + 1 << ": SyntaxError: unexpected EOF while parsing");
                  }
                  else
                  {
                     //get rid of the '\'
                     line->resize(line->size() - 1);

                     //Move the next line to current line
                     line->insert(line->end(), nextLine->begin(), nextLine->end());

                     //erase the empty next line
                     DocumentLines.erase(nextLine);

                     goto GOTO_CHECK_FOR_LINE_CON_CHAR;
                  }
               }
               else
               {
                  const auto &Token = (*(turtle::TokenDataStructure *)pos->NodePtr);
                  printError("Line " << Token.LineIndex + 1 << ": SyntaxError: unexpected character after line continuation character");
               }
            }
         }

         //search for ';'
         /*for (auto line = DocumentLines.begin(); line != DocumentLines.end(); ++line)
         {
            const auto &pos = std::find_if(line->cbegin(), line->cend(), [&](const auto &NodePairToken) {
               const char &c = ((turtle::TokenDataStructure *)NodePairToken.NodePtr)->TokenString[0];
               if (c == ';')
               {
                  return true;
               }
               return false;
            });
            if (pos != line->cend() && pos != line->cend() - 1)
            {

            }
         }*/

#ifdef DEBUG
         puts("\nLogical lines");
         for (auto &a : DocumentLines)
         {
            for (auto &b : a)
            {
               std::cout << ((turtle::TokenDataStructure *)b.NodePtr)->TokenString << ' ';
            }
            puts("");
         }
#endif
      }
      return EXIT_SUCCESS;
   });

#ifdef DEBUG
   puts("Done");
   std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << " milliseconds (0.001s)";
#endif

   return EXIT_SUCCESS;
}
