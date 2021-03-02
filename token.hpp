//better to look like an idiot by overcommenting code,
//then to be the idiot without comments in their code

#ifndef MAIN_H
#define MAIN_H
#include <regex>
#include <iostream>
#include <any>
#include <array>
#include <any>
#include <algorithm>
#include <execution>
#include "node.hpp"
#include "global.h"
#include "commen.h"
#include "ctre.hpp"

namespace turtle {

void help();

//return token flag
turtle_flag findToken(const char *__restrict str)
{
    if (strlen(str) <= 8)
    {
        const uint_fast64_t hash = sti(str);
        for (uint_fast8_t i = 0; i < lengthof(turtleBuiltinTokenMap); ++i)
        {
            if (hash == turtleBuiltinTokenMap[i][0])
            {
                return  turtleBuiltinTokenMap[i][1];
            }
        }
    }
    return token::flag::Control::NULL_TOKEN;
}

#if DEBUG_CPP

//for debugging
std::string getFlagName(turtle_flag flag)
{
    switch(flag >> tokenTypeOffset){
    case token::Type::CONTROL:
        if(hasFlag(flag, token::flag::Control::NEWLINE)){
            //return (std::string("NEWLINE: WS -> ")+ std::to_string((flag ^ token::flag::Control::NEWLINE)));
            return "NEWLINE";
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
        return turtle::flagstr(token::flag::Data::_values(), flag);
        break;
    case token::Type::IDENTIFIER:
        return "IDENTIFIER";
        break;
    }
    return "";
}
#endif

void lex(turtle::Document &Document){

#if DEBUG_CPP
    constexpr static const char * character_substitutions[][2] = {
        {R"(\n)", R"(\n)"},
        {R"(\r)", R"(\r)",},
        //replace tab with 8 characters
        {R"(\t)", R"(\t------)"}
    };
    //print out tokens that were captured
    for(unsigned int i = 0; i < (unsigned int)Document.Lexemes.size(); ++i){
        auto & tok = Document.Lexemes[i].str;
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

    Document.Nodes.reserve(Document.Lexemes.size());
    for(auto& Lexeme : Document.Lexemes){
        auto & Lstr = Lexeme.str;
        Node tmpNode;
        tmpNode.NodeFlags = turtle::token::flag::Control::NULL_TOKEN;

        switch(Lstr[0]){
        case  0  ...   8:
        case 11  ...  12:
        case 14  ...  31:
        case         127:
            panic("Line %d:%d: ERR unexpected token\n", Lexeme.lnum + 1, Lexeme.lpos);
            break;

        //ignore comments and backslashes
        case '\\':
        case '#':
            break;

        case '\n':
        case '\r':{
            unsigned int len = Lstr.size();
            if(len > 1){
                for(unsigned int j = 1; j < len; ++j){
                    if(Lstr[j] == '\t'){
                        len -= 1; //remove tab character
                        len += 8; //replace with 8 spaces
                    }
                }
            }
            tmpNode.NodeFlags |= turtle::token::flag::Control::NEWLINE;
            tmpNode.header |= (len-1);
            }break;
        case '.':{
            if(Lstr.back() == '.'){
                goto _DEFUALT_FIND_TOKEN;
            }
        }
        //fucking numbers
        case '0' ... '9':{
            #define num_pattern(PATTERN)                                        \
                [&](const std::string_view & str){                              \
                    static constexpr ctll::fixed_string re{PATTERN};            \
                    return ctre::match<re>(static_cast<std::string_view>(str)); \
                }
            enum num_types {
                INT,
                FLOAT,
                EXPONENTIAL,
                COMPLEX,
                HEX,
                OCTAL,
                BINARY,
            };
            using namespace turtle::token;
            const static std::pair<std::function<bool(const std::string_view&)>, turtle::turtle_flag> regexes[] = {
                [INT]         = {num_pattern(R"(^[0-9_]+$)"),               flag::Data::DATA_TYPE_INT},
                [FLOAT]       = {num_pattern(R"(^[0-9._]+$)"),              flag::Data::DATA_TYPE_FLOAT},
                [HEX]         = {num_pattern(R"(^0[xX]+[0-9A-Fa-f_]+$)"),   flag::Data::DATA_TYPE_HEX},
                [OCTAL]       = {num_pattern(R"(^0[oO]+[0-9A-Fa-f_]+$)"),   flag::Data::DATA_TYPE_OCTAL},
                [BINARY]      = {num_pattern(R"(^0[bB]+[01_]+$)"),          flag::Data::DATA_TYPE_BINARY},
                [COMPLEX]     = {num_pattern(R"(^[0-9._]+[jJ]+$)"),         flag::Data::DATA_TYPE_COMPLEX,},
                [EXPONENTIAL] = {num_pattern(R"(^[0-9_]+[eE]+-?[0-9_]+$)"), flag::Data::DATA_TYPE_EXPONENTIAL},
            };
            #undef num_pattern
            auto it = std::begin(regexes);

            //if greater than 1 digit check what kind of number
            if(Lstr.size() > 1){
                it = std::find_if(std::execution::par, it, std::end(regexes), [&](const auto& regex){
                    return regex.first(Lstr);
                });
                if(it == std::end(regexes)){
                    panic("Invalid number literal %s\n", Lstr.c_str());
                }
            }
            tmpNode.NodeFlags |= it->second;
            tmpNode.header    |= Document.data.size();
            Document.data.push_back(Lstr);
        }break;
        //fucking strings
        case 'a' ... 'z':
        case 'A' ... 'Z':{
            if(Lstr.size() > 1){
                for(unsigned int j = 1; j < 3; ++j){
                    if(Lstr[j] == '"' || Lstr[j] == '\''){
                        //if(lexeme starts with [rRuUfF] and has ('"' or '\'') ) {goto _TOKEN_STRING_WITH_PREFIX;}
                        goto _TOKEN_STRING_WITH_PREFIX;     // ─┐  jump down
                    }                                       //  │
                }                                           //  │
            }                                               //  │
            //           else{ goto _DEFUALT_FIND_TOKEN; }  //  │
            goto _DEFUALT_FIND_TOKEN;  // jump down  ───────────┼──────────────────────────────────────────────────────────────>┐
                                       //                       │                                                               │
            _TOKEN_STRING_WITH_PREFIX: //   <───────────────────┘                                                               │
            enum {                                                                                                           // │
                NULL_FLAG     = 0b000,                                                                                      // │
                RAW_FLAG      = 0b001,                                                                                      // │
                UNICODE_FLAG  = 0b010,                                                                                      // │
                FORMATED_FLAG = 0b100,                                                                                      // │
            };                                                                                                               // │
                                                                                                                             // └>───────────────────────────────>┐
            //Give an any unspecified elements NULL_FLAG
            constexpr static uint_fast8_t fmap[SCHAR_MAX] = {
                      ['R'] =      RAW_FLAG,
                      ['r'] =      RAW_FLAG,
                      ['f'] = FORMATED_FLAG,
                      ['F'] = FORMATED_FLAG,
                      ['U'] =  UNICODE_FLAG,
                      ['u'] =  UNICODE_FLAG
            };
            using namespace turtle::token;
            //Give an any unspecified elements flag::Control::NULL_TOKEN
            constexpr static turtle_flag tmap[] = {
                [RAW_FLAG]                     = flag::Data::DATA_TYPE_RAW_STRING,              // 1 0b001
                [UNICODE_FLAG]                 = flag::Data::DATA_TYPE_UNICODE_STRING,          // 2 0b010
                [FORMATED_FLAG]                = flag::Data::DATA_TYPE_FORMATED_STRING,         // 4 0b100
                [UNICODE_FLAG | FORMATED_FLAG] = flag::Data::DATA_TYPE_FORMATED_UNICODE_STRING  // 6 0b110
            };
            constexpr const char * premap [] = {
                [RAW_FLAG | FORMATED_FLAG] = "raw formatted",
                [RAW_FLAG |  UNICODE_FLAG] = "raw unicode",
            };
            //sexy, instantaneous
            uint_fast8_t f = 0;
            for(auto * c = Lstr.data(); !(*c == '"' || *c == '\''); ++c){
                f ^= *(fmap + *c);
                switch(f){
                    case RAW_FLAG |  UNICODE_FLAG:
                    case RAW_FLAG | FORMATED_FLAG:
                        panic("Line %d:%d Theres no such thing as a %s string\n", Lexeme.lnum + 1, Lexeme.lpos, premap[f]);
                        break;
                    case NULL_FLAG:
                        panic("Line %d:%d Invalid string prefix\n", Lexeme.lnum + 1, Lexeme.lpos);
                        break;
                }
            }
            tmpNode.NodeFlags |= tmap[f];
                                                                                                                                                               // │
        }                                                                                                                                                      // │
        [[fallthrough]];                                                                                                                                       // │
        case '"':                                                                                                                                              // │
        case '\'':{                                                                                                                                            // │
            constexpr static ctll::fixed_string invalid_string{R"([a-zA-Z]{0,2}?("|')(\1{2})?)"};
            if(ctre::match<invalid_string>(static_cast<std::string_view>(Lstr))){                                                                              // │
                panic("Line %d:%d Non terminating string\n", Lexeme.lnum + 1, Lexeme.lpos);                                                                    // │
            }                                                                                                                                                  // │
            tmpNode.NodeFlags |= turtle::token::flag::Data::DATA_TYPE_STRING;                                                                                  // │
            tmpNode.header |= Document.data.size();
            Document.data.push_back((std::string_view)Lstr);                                                                                                   // │
            }break;                                                                                                                                            // │
        default:                                                                                                                                               // │
            _DEFUALT_FIND_TOKEN: // <────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────<┘
            //turtle::findToken returns 0 if not found in the predefined tokens map
            if( (tmpNode.NodeFlags |= turtle::findToken(Lstr.c_str())) ){
                break;
            }
            tmpNode.NodeFlags |= turtle::token::flag::Type::IDENTIFIER;
            tmpNode.header    |= Document.data.size();
            Document.data.push_back(Lstr);
            break;
        }

#if DEBUG_CPP
        auto whitespace = [](const size_t& size){
            return std::string(size, ' ');
        };
        std::string res = Lstr;
        if(res[0] == '\n' || res[0] == '\r'){
            for(auto& ex : character_substitutions){
                res = std::regex_replace(res, std::regex(ex[0]), ex[1]);
            }
        }
        std::ostringstream snake_y;
        snake_y << std::bitset< sizeof(turtle_flag) * 8 >(tmpNode.NodeFlags);
        const std::string& flag_str = getFlagName(tmpNode.NodeFlags);

        //                                                                                                       ~~ 33 spaces ~~
        snake_y << " | Predicted token -> " << flag_str << (flag_str.size() + static_cast<const char *>("                                 "))
                << " | token -> [";

        //if string, format it so stays on the right margin instead of wraping in the console preview
        if(tmpNode.hasFlag(turtle::token::flag::Data::DATA_TYPE_STRING)){
            static const std::regex regex(R"(\n|\r)");
            std::vector<std::string> lines(
                std::sregex_token_iterator(res.begin(), res.end(), regex, -1),
                {}
            );
            const auto lnbuffsize = snake_y.str().size();
            for(uint_fast8_t i = 0; i < (uint_fast8_t)lines.size(); ++i){
                if(i){snake_y << '\n' << whitespace(lnbuffsize);}
                snake_y <<  lines[i];
            }
        } else {
            snake_y << res;
        }
        //oh shit its a snakeeeee
        std::cout << snake_y.str() << "]\n";

#endif // ifdef DEBUG
        tmpNode.string = std::move(Lstr);
        tmpNode.line = std::move(Lexeme.lnum);
        tmpNode.linepos = std::move(Lexeme.lpos);
        Document.Nodes.push_back(tmpNode);
    }
}

} // namespace turtle

#endif // MAIN_H
