#ifndef MAIN_H
#define MAIN_H
#include <regex>
#include <iostream>
#include <any>
#include "node.hpp"
#include "global.h"

void help();

struct DocumentBucket
{
    //Document Nodes
    std::vector<struct Node> Nodes;
    struct dataObject
    {
        __int128 num = 0;
        std::string str = "";
    };
    std::vector<dataObject> data;
    unsigned int size = 0;
    Node *head = 0;
};

auto lexFile(std::string &filedata, DocumentBucket &Document)
{
    std::locale::global(std::locale("en_US.UTF-8"));
    //([rRfFUu]{0,2}"""(?:[^\\"]|\\.)*"""|([rRfFUu]{0,2}?'(?:[^\\']|\\.)*'|[\w]{0,2}?"(?:[^\\"]|\\.)*")|(#[^\r\n]*)|(\n[ \t]*)|(\\[^\r\n]*)|^|(([<>*\/]{2}=?)|([=<>!+\-*\\&|]=))|([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+))
    std::regex TokenRegex(
        //Capture doc strings
        R"(([rRfFUu]{0,2}?"""(?:[^\\"]|\\.)*""")"

        //Capture single ' and double " qoute strings
        "|"
        R"(([rRfFUu]{0,2}?'(?:[^\\']|\\.)*'|[rRfFUu]{0,2}?"(?:[^\\"]|\\.)*"))"

        //Capture python single comment
        "|"
        R"((#[^\r\n]*))"

        //Capture newline and its whitespace
        "|"
        R"((\n[ \t]*))"

        //Capture backslash '\' and anything that comes after it, so we can quickly ignore it
        "|"
        R"((\\[^\r\n]*))"

        //Capture just newline if we haven't in the previos step
        "|"
        R"(^)"

        //Capture 2 character operators such as +=
        "|"
        R"((([<>*\/]{2}=?)|([=<>!+\-*\\&|]=)))"

        //capture anything else, symbols, and any other arithmitic
        "|"
        R"(([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+)))"

    );

    std::regex_token_iterator<std::string::iterator> rend, rit(filedata.begin(), filedata.end(), TokenRegex);
    ++rit; // The first & last token is always "" so skip over it

    Document.Nodes.reserve(std::distance(rit, rend));
    Node tmpNode;

    tmpNode.NodeFlags = turtle::token::flag::NEWLINE;
    Document.Nodes.push_back(tmpNode);

    for (; rit != rend; ++rit)
    {
        const auto &token = rit->str();
        DEBUG_M(printf("[%s]\n", token.c_str()));
        tmpNode.NodeFlags = 0;
        switch (token[0])
        {
        /* ignore '\' backslashes and anything after them */
        case '\\':
        {
            break;
        }
        case '\n':
        case '\r':
        {

            /*
             *    ┌──> Flag Type - Control Class Id
             *    │                           ┌──> Amount Of whitespace / scope (11 bits -> max 2,048 characters)
             * ┌──┤                   ┌───────┴──┐┌───> Is newline
             * 00000000  00000000 00001111 11111111
             *
             */

            // The regex matches the newline + whitespace at the beginning,
            // so subtract 1 and to get how much whitespace there is
            tmpNode.NodeFlags |= ((token.size()) << 1) | turtle::token::flag::NEWLINE;
            break;
        }

        case '(':
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_LEFT_BRACE;
            [[fallthrough]];
        case ')':
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_CURVED_BRACE;
            break;

        case '{':
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_LEFT_BRACE;
            [[fallthrough]];
        case '}':
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_CURLY_BRACE;
            break;

        case '[':
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_LEFT_BRACE;
            [[fallthrough]];
        case ']':
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_SQUARE_BRACE;
            break;

        case ';':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_SEMICOLON;
        }
        break;

        case '#':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DATA_TYPE_COMMENT;
            Document.data.push_back({0, std::move(token)});
            break;
        }
        case ',':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_COMMA;
        }
        break;

        //Although I'm not going to program this in now cuz I don't use the '@', I'll classify it anyway
        case '@':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_AT_SIGN;
        }
        break;
        case ':':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_COLON;
        }
        break;
        case '.':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DELIMITER_PERIOD;
        }
        break;

        //there is no '~=' operator in python so it is safe to put it in here
        case '~':
        {
            tmpNode.NodeFlags |= turtle::token::flag::ARITHMETIC_BIT_NOT;
        }
        break;

        case '0':
            if (token.size() >= 3)
            {
                switch (token[1])
                {
                case 'x':
                {
                    goto PUSH_BACK_LEX_TOKEN_GT;
                }
                break;
                case 'o':
                {
                    goto PUSH_BACK_LEX_TOKEN_GT;
                }
                break;
                case 'b':
                {
                    goto PUSH_BACK_LEX_TOKEN_GT;
                }
                break;
                case 'e':
                {
                    goto PUSH_BACK_LEX_TOKEN_GT;
                }
                break;
                }
            }
            [[fallthrough]];
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            tmpNode.NodeFlags |= turtle::token::flag::DATA_TYPE_NUMBER;
            Document.data.push_back({stoll(std::move(token)), ""});
        }
        break;

        //to do implement strings

        //unicode
        case '"':
        case '\'':
        {
            if (token.size() >= 2)
            {
                tmpNode.NodeFlags |= turtle::token::flag::DATA_TYPE_STRING;
                Document.data.push_back({0, token});
                break;
            }
            else
            {
                panic("Non terminating string");
            }
        }
        case 'u':
        case 'U':
            //if a valid string
            //the regex will not match the string and its prefix together if the string is not terminated
            switch (token.back())
            {
            case '"':
            case '\'':

                break;
            }
            goto IDENTIFIER_LEX_TOKEN_GT;
            break;
        case 'f':
        case 'F':
        //raw strings
        case 'r':
        case 'R':
        {
            if (token.size() >= 3 && (token[1] == '\'' || token[1] == '"'))
            {
                tmpNode.NodeFlags |= turtle::token::flag::DATA_TYPE_RAW;
                break;
            }

            //if not raw string fallthough
            //  example: 'range' starts w/ 'r' so it would be picked up by the above,
            //           but does not pass if statment so fallthough
        }
            [[fallthrough]];
        default:
        {

            if (token.size() <= 8)
            {
                if ((tmpNode.NodeFlags |= turtle::findKeyword(token.c_str())))
                {
                    break;
                }
            }
            [[fallthrough]];
        //there are no reserved keywords that start with these letters
        case 'h':
        case 'j':
        case 'k':
        case 'm':
        case 'q':
        case 's':
        case 'v':
        case 'x':
        case 'z':

        //no reserved keywords start with these uppercase letters
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
            //case 'N': --> None --> classify as data
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'S':
            //case 'T': --> True
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        {
        IDENTIFIER_LEX_TOKEN_GT:
            Document.data.push_back({0, token});
            tmpNode.NodeFlags |= turtle::token::flag::IDENTIFIER ^ (Document.data.size() - 1);
        }
        }
        }
    PUSH_BACK_LEX_TOKEN_GT:
        Document.Nodes.push_back(tmpNode);
    }
}

void printLexTokens(DocumentBucket &Document)
{
    for (unsigned int i = 0; i < Document.Nodes.size(); ++i)
    {
        std::cout << std::bitset<32>(Document.Nodes[i].NodeFlags) << ' ';
        //printf("%d\n", Document.Nodes[i].NodeFlags);
        switch (Document.Nodes[i].NodeFlags >> (32 - 4))
        {
        case turtle::token::CONTROL:
            printf("Newline: %d\n", Document.Nodes[i].NodeFlags >> 1);
            break;
        case turtle::token::KEYWORD:
            puts("Kw");
            break;

        case turtle::token::IDENTIFIER:
        {
            turtle_flag id = turtle::token::flag::IDENTIFIER ^ Document.Nodes[i].NodeFlags;
            printf("Id: %u -> %s\n", id, Document.data[id].str.c_str());
        }
        break;
        case turtle::token::DATA:
            puts("DATA");
            break;
        case turtle::token::ARITHMETIC:
            puts("Math");
            break;
        case turtle::token::DELIMITERS:
            puts("Delim");
            break;
        default:
            std::cout << Document.Nodes[i].NodeFlags << '\n';
            break;
        }
    }
}

void start(int argc, char *argv[])
{
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
