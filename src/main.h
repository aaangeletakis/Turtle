#ifndef MAIN_H
#define MAIN_H

#include <regex>
#include "magic.hpp"
#include <iostream>

void help();


struct Node {
    //Node
    uint32_t NodeFlags = 0;
    Node *Parent = 0;
    Node *Children[2] = {0};
};

struct DocumentBucket {
    //Document Nodes
    std::vector<struct Node> Nodes;
    std::vector <std::string> data;
};

auto getPhysicalLineTokens(std::string &filedata, DocumentBucket &Document) {

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
            R"(|([!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]|[^\s!"\#%&'()*+,\-.\/:;<=>?@\[\\\]^‘{|}~]+)))"

    );

    std::sregex_token_iterator rend, rit(filedata.begin(), filedata.end(), TokenRegex);
    ++rit; // The first token is always "" so skip over it

    Document.Nodes.reserve(std::distance(rit, rend));

    Node Node;

    for (int lineIndex = 0; rit != rend; ++rit) {
        const auto &token = rit->str();
        printf("[%s]\n", token.c_str());

        Node.NodeFlags = 0;
        switch (token[0]) {
            /* ignore '\' */
            case '\\': {
                break;
            }
            case '\n':
            case '\r': {

                /*
                 *    ┌──> Flag Type - Control Class Id
                 *    │                           ┌──> Amount Of whitespace (11 bits -> max 2,048 characters)
                 * ┌──┤                   ┌───────┴──┐┌───> Is newline
                 * 00001111  11111111 11111111 11111111
                 *     │                 │
                 *     └─────────────────┴──> Line Number (16 bits -> max 65,536 lines)
                 */

                // The regex matches the newline + whitespace at the beginning,
                // so subtract 1 and to get how much whitespace there is
                Node.NodeFlags |= (++lineIndex << 12) | ((token.size() - 1) << 1) | turtle::token::flag::NEWLINE;
                Document.Nodes.push_back(Node);
                break;
            }

            case '(':
                Node.NodeFlags |= turtle::token::flag::DELIMITER_LEFT_BRACE;
                        __attribute__ ((fallthrough)); // Tell GCC this is intentional
            case ')':
                Node.NodeFlags |= turtle::token::flag::DELIMITER_CURVED_BRACE;
                break;

            case '{':
                Node.NodeFlags |= turtle::token::flag::DELIMITER_LEFT_BRACE;
                        __attribute__ ((fallthrough)); // Tell GCC this is intentional
            case '}':
                Node.NodeFlags |= turtle::token::flag::DELIMITER_CURLY_BRACE;
                break;

            case '[':
                Node.NodeFlags |= turtle::token::flag::DELIMITER_LEFT_BRACE;
                        __attribute__ ((fallthrough)); // Tell GCC this is intentional
            case ']':
                Node.NodeFlags |= turtle::token::flag::DELIMITER_SQUARE_BRACE;
                break;

            case ';': {
                Node.NodeFlags |= turtle::token::flag::DELIMITER_SEMICOLON;
                break;
            }
            case '#': {
                Node.NodeFlags |= turtle::token::DATA_TYPE_COMMENT;
                Document.data.push_back(token);
                Document.Nodes.push_back(Node);
                break;
            }
            case ',': {
                Node.NodeFlags |= turtle::token::DELIMITER_COMMA;
                break;
            }

                //Although I'm not going to program this in now cuz I don't use the '@', I'll classify it anyway
            case '@': {
                Node.NodeFlags |= turtle::token::DELIMITER_AT_SIGN;
                break;
            }
            case ':': {
                Node.NodeFlags |= turtle::token::DELIMITER_COLON;
                break;
            }
            case '.': {
                Node.NodeFlags |= turtle::token::flag::DELIMITER_PERIOD;
                break;
            }

            case 'r':
                        __attribute__ ((fallthrough)); // Tell GCC this is intentional
            case 'R': {
                if (token.size() > 2 && (token[1] == '\'' || token[1] == '"')) {
                    Node.NodeFlags |= turtle::token::flag::DATA_TYPE_RAW;
                            __attribute__ ((fallthrough));
                    case '"':
                    case '\'': {
                        Node.NodeFlags |= turtle::token::flag::DATA_TYPE_STRING;
                        Document.data.push_back(token);
                        Document.Nodes.push_back(Node);
                    }

                    break;
                }/*else exit and print error non terminating string"*/
                        __attribute__ ((fallthrough));
            }
            default: {
                for (unsigned int i = 0; i < 65; ++i) {
                    if (token == turtle::turtleBuiltinTokenMap[i].TokenString) {
                        Node.NodeFlags = turtle::turtleBuiltinTokenMap[i].TokenFlags;
                        i += 66;
                    }
                }

                if (Node.NodeFlags == turtle::token::flag::NULL_TOKEN) {
                    printf("[%s] is id", token.c_str());
                    Document.data.push_back(token);
                    Node.NodeFlags |= turtle::token::flag::IDENTIFIER | (Document.data.size() - 1);
                }
                break;
            }
        }
        Document.Nodes.push_back(Node);
    }
}

void convertToLogicalLines(DocumentBucket &Document) {
    //first pass is for
    for (unsigned int i = 0; i < Document.Nodes.size(); ++i) {
        std::cout << Document.Nodes[i].NodeFlags << '\n';
    }
}

void start(int argc, char *argv[]) {
    switch (argc) {
        case 1: {
            help();
            exit(EXIT_SUCCESS);
            break;
        }
        case 2: {
            if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
                help();
                exit(EXIT_SUCCESS);
            }
            break;
        }
        default: {
            std::cerr << "To many args\n";
            exit(EXIT_SUCCESS);
            break;
        }
    }
}

void help() {
    printf("Turtle.exe [filename]\n"
           "Translate python source code to C++ source code\n");
    exit(EXIT_SUCCESS);
}

#endif // MAIN_H
