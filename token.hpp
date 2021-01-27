#ifndef MAIN_H
#define MAIN_H
#include <regex>
#include <iostream>
#include <any>
#include "node.hpp"
#include "global.h"
#include <array>
#include <any>


namespace turtle {

void help();

namespace type_ids {
enum {
    LONG_DOUBLE,
    DOUBLE,
    FLOAT
};
enum {
    INT128,
    LONG_LONG_INT,
    LONG_INT,
    SHORT_INT,
    CHAR
};
}

//slowwwwwwwwwwwwww
//there is a way to check for overflow in ASM but is non portable
unsigned char getIntagerType(const __int128 &i){
    //check which type overflows
    __int128 types[] = {
             (__int128)i, //go down the ladder
        (long long int)i,
             (long int)i,
                  (int)i,
            (short int)i,
    };
    unsigned char v=0;
    while(i == types[v]) ++v;
    return v;
}

auto getDecimalType(const long double &d){
    //check which type overflows
    long double types[] = {
             (long double)d,
                  (double)d,
                   (float)d,
    };
    unsigned char v=0;
    while(d == types[v++]);
    return v;
}

struct match {
    std::string token;
    const uint_fast16_t lpos = 0;
    const uint_fast16_t lnum = 0;
};

struct Document
{
    //Document Nodes
    std::vector<struct Node> Nodes;
    std::vector<match> matches;
    std::vector<std::any> data;
};

auto tokenize(std::string &filedata, std::vector<match> &matches)
{
#include "regex_macros_def.h"
/*
[rRfFUu]{0,2}?"{3}(?:[^\\"]|\\.)*"{3}
|[rRfFUu]{0,2}?'{3}(?:[^\\']|\\.)*'{3}
|([rRfFUu]{0,2}?'(?:[^\\']|\\.)*'
|[rRfFUu]{0,2}?"(?:[^\\"]|\\.)*")
|(#[^\r\n]*)
|([\n\r][ \t]*)
|(\\[^\r\n]*)
|(([<>*\/]{2})=?
|([!%&*+\-<=>\/\\^|]=))
|([!-\/:-@\[-^{-~]|[^\s!-\/:-@\[-^{-~]+)
*/
    //when rEgEX is A LaNGUAgE
    static std::regex TokenRegex(
            //Capture doc strings """

            //this rule needs to have higher precedence than regular strings
            //or else the matcher will get confused between the difference of """ and "

       __STRING_PREFIX ___DOC_STRING(__DOUBLEQ)
       __ELSE(
             __STRING_PREFIX ___DOC_STRING(__SINGLEQ)
       )
       //Capture strings
       __ELIF(
            //Capture single ' quote strings
            //string capture
            __STRING_PREFIX ___STRING(__SINGLEQ)

            __ELSE(
                //Capture double " qoute strings
                //string capture
                __STRING_PREFIX ___STRING(__DOUBLEQ)
            )

       )

            //Capture python single comment
       __ELIF(
              "#" ___AND __ZERO_OR_MORE_OF(__ANY_CHAR_THATS_NOT_A_NEWLINE)
       )

       //Capture newline and its whitespace
       __ELIF(
               __NEWLINE ___AND __ZERO_OR_MORE_OF(__WHITESPACE)
       )

       //Capture backslash '\' and anything that comes after it, so we can quickly ignore it
       __ELIF(
              __BACKSLASH ___AND __ZERO_OR_MORE_OF(__ANY_CHAR_THATS_NOT_A_NEWLINE)
       )

       //Capture operators such as +=
       __ELIF(
              //capture 2-3 character assignment operators i.e. <<= or <=
              R"(([<>*\/]{2}))" ___AND __ZERO_OR_ONE_OF("=")
              ___ELSE(
                    //capture 2 character operators i.e. ==
                    __ANY_CHAR_OF(R"(!%&*+\-<=>\/\\^|)") ___AND "="
              )
       )

       //capture anything else, symbols, labels, and any other arithmitic
       ___ELSE(
                 //capture any single character symbol/operator i.e. =
                 __ANY_CHAR_OF(__PYTHON_SYMBOLS)
                 __ELSE(
                     //capture any identifiers/numbers
                     //i.e anything not a sybol or space
                     __ONE_OR_MORE_OF( __NONE_OF(R"(\s)" __PYTHON_SYMBOLS ) )
                 )
       )
    );
#include "regex_macros_undef.h"

    std::sregex_iterator rend, rit(filedata.begin(), filedata.end(), TokenRegex);
    matches.reserve(std::distance(rit, rend));

    unsigned int ln = 0, nl_pos = 0;
    for (; rit != rend; ++rit){
        const auto& str = rit->str();
        if(str[0] == '\n' || str[0] == '\r'){
            ++ln;
            nl_pos = rit->position();
        }
        matches.push_back({str, (uint_fast16_t)(rit->position() - nl_pos), ln});
    }
}

#ifdef DEBUG
//for debugging
std::string getFlagName(turtle_flag flag)
{
    switch(flag >> ((sizeof(turtle_flag) * 8) - 3)){
    case token::Type::CONTROL:
        if(hasFlag(flag, token::flag::Control::NEWLINE)){
            return (std::string("NEWLINE: WS -> ")+ std::to_string((flag ^ token::flag::Control::NEWLINE)));
        }
        return flagstr(token::flag::Control::_values(), flag);
        break;
    case token::Type::DELIMITERS:
        return flagstr(token::flag::Operator::_values(), flag);
        break;
    case token::Type::ARITHMETIC:
        return flagstr(token::flag::Arithmetic::_values(), flag);
        break;
    case token::Type::KEYWORD:
        return flagstr(token::flag::Keyword::_values(), flag);
        break;
    case token::Type::DATA:
        return turtle::flagstr(token::flag::Data::_values(), ((flag >> 16) << 16));
        break;
    case token::Type::IDENTIFIER:
        return "IDENTIFIER";
        break;
    }
    return "";
}
#endif

void lex(turtle::Document &Document){

#ifdef DEBUG
    constexpr const char * character_substitutions[][2] = {
        {R"(\n)", R"(\n)"},
        {R"(\r)", R"(\r)",},
        //replace tab with 8 characters
        {R"(\t)", R"(\t------)"}
    };
    //print out tokens that were captured
    for(unsigned int i = 0; i < (unsigned int)Document.matches.size(); ++i){
        auto & tok = Document.matches[i].token;
        std::string res = tok;
        if(res[0] == '\n' || res[0] == '\r'){
            for(auto& ex : character_substitutions){
                res = std::regex_replace(res, std::regex(ex[0]), ex[1]);
            }
            printf("\n");
        }
        printf("[%s]", res.c_str());
    }
    printf("\n");
#endif // ifdef DEBUG

    Document.Nodes.reserve(Document.matches.size());
    for(auto& match : Document.matches){
        auto & tok = match.token;
        Node tmpNode;
        auto & flag = tmpNode.NodeFlags;
        flag = turtle::token::flag::Control::NULL_TOKEN;
        switch(tok[0]){
        case  0  ...   8:
        case 11  ...  12:
        case 14  ...  31:
        case         127:
            panic("Line %d:%d: ERR unexpected token\n", match.lnum + 1, match.lpos + 1);
            break;

        //ignore comments and backslashes
        case '\\':
        case '#':
            break;

        case '\n':
        case '\r':{
            unsigned int len = tok.size();
            if(len > 1){
                for(unsigned int j = 1; j < len; ++j){
                    if(tok[j] == '\t'){
                        len -= 1;
                        len += 8;
                    }
                }
            }
            flag |= turtle::token::flag::Control::NEWLINE | (len-1);
            }break;
        case '"':
        case '\'':{
            unsigned int len = tok.size();
            if(len == 1){
                panic("Line %d:%d Non terminating string %c\n", match.lnum + 1, match.lpos + 1, tok[0]);
            }
            flag |= turtle::token::flag::Data::DATA_TYPE_STRING | Document.data.size();
            Document.data.push_back((std::string_view)tok);
            }break;
        //fucking numbers
        case '0' ... '9':{
            flag |= turtle::token::flag::Data::DATA_TYPE_NUMBER | Document.data.size();
            Document.data.push_back((std::string_view)tok);
        }break;
        //fucking strings
        case 'r':
        case 'R':
        case 'u':
        case 'U':
        case 'f':
        case 'F':{
            if(tok.size() > 2){
                for(unsigned int j = 1; j < 3; ++j){
                    if(tok[j] == '"' || tok[j] == '\''){
                        goto _TOKEN_STRING_WITH_PREFIX; // jump down
                    }
                }
            }
            goto _DEFUALT_FIND_TOKEN; // jump down

            _TOKEN_STRING_WITH_PREFIX:
            enum {
                _RAW,
                _UNICODE,
                _FORMATED,
                _RAW_FLAG      = 0b001,
                _UNICODE_FLAG  = 0b010,
                _FORMATED_FLAG = 0b100,
            };
            //sexy
            uint_fast8_t f = 0;
            for(uint_fast8_t j=0; j < 3; ++j){
                switch(tok[j]){
                case 'r':
                case 'R':{
                    f ^= _RAW_FLAG;
                    }break;
                case 'U':
                case 'u':{
                    f ^= _UNICODE_FLAG;
                    }break;
                case 'f':
                case 'F':{
                    f ^= _FORMATED_FLAG;
                    }break;
                case '"':
                case '\'':
                    goto _ASSIGN_STRING_PREFIX; //jump down
                    break;
                }
                if(f == 0){
                    panic("Line %d:%d String prefix specified twice\n", match.lnum + 1, match.lpos + 1);
                }
            }
            panic("Something went wrong at line %d in string prefix section\n"
                  "token -> [%s]\n", __LINE__, tok.c_str());

            _ASSIGN_STRING_PREFIX:
            switch(f){
            case _RAW_FLAG     | _FORMATED_FLAG:{
                panic("Line %d:%d There is no such thing as a raw formatted string!\n", match.lnum + 1, match.lpos + 1);
                }break;
            case _RAW_FLAG     | _UNICODE_FLAG:{
                panic("Line %d:%d There is no such thing as a raw unicode string!\n", match.lnum + 1, match.lpos + 1);
                }break;
            case _UNICODE_FLAG | _FORMATED_FLAG:{
                flag |= turtle::token::flag::Data::DATA_TYPE_FORMATED_UNICODE_STRING;
                }break;
            case _FORMATED_FLAG:{
                flag |= turtle::token::flag::Data::DATA_TYPE_FORMATED_STRING;
                }break;
            case _RAW_FLAG:{
                flag |= turtle::token::flag::Data::DATA_TYPE_RAW_STRING;
                }break;
            case _UNICODE_FLAG:{
                flag |= turtle::token::flag::Data::DATA_TYPE_UNICODE_STRING;
                }break;
            }

        }break;
        default:
            _DEFUALT_FIND_TOKEN:
            if( (flag |= turtle::findToken(tok.c_str())) ){
                break;
            }
            flag |= turtle::token::flag::Type::IDENTIFIER | Document.data.size();
            Document.data.push_back(tok);
            break;
        }

#ifdef DEBUG
        std::string res = tok;
        if(res[0] == '\n' || res[0] == '\r'){
            for(auto& ex : character_substitutions){
                res = std::regex_replace(res, std::regex(ex[0]), ex[1]);
            }
        }
        std::ostringstream ss;
        ss << std::bitset< sizeof(turtle_flag)*8 >(tmpNode.NodeFlags);
        const std::string& flag_str = getFlagName(tmpNode.NodeFlags);
        {
            std::string ws; //                        The token with the largest name is 32 characters long
            for(int i=0, size = flag_str.size(); i < (33 - size); ++i){ws+=' ';}
            ss << " | Predicted token -> " << flag_str << ws
               << " | token -> [";
        }
        //if string, format it so stays on the right margin instead of wraping in the console preview
        if(res[0] == '"' || res[0] == '\''){
            std::regex regex{R"(\n)"};
            std::sregex_token_iterator it{res.begin(), res.end(), regex, -1};
            std::vector<std::string> lines{it, {}};
            for(int j = 0; j < (int)lines.size(); ++j){
                if(j > 0){
                    ss << '\n';
                }
                ss << lines[j];
            }
        } else {
            ss << res;
        }

#endif // ifdef DEBUG
        tmpNode.line = std::move(match.lnum);
        tmpNode.linepos = std::move(match.lpos);
#ifdef DEBUG
        std::cout << ss.str() << "] \n";
#endif
        Document.Nodes.push_back(tmpNode);
    }
}

} // namespace turtle

#endif // MAIN_H
