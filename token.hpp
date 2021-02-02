//better to look like an idiot by overcommenting code,
//then to be the idiot without comments in their code

#ifndef MAIN_H
#define MAIN_H
#include <regex>
#include <iostream>
#include <any>
#include "node.hpp"
#include "global.h"
#include <array>
#include <any>
#include <algorithm>

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
//there is a way to check for overflow in ASM, but is non portable
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

struct _Lexeme  {
    std::string str;
    const uint_fast16_t lpos = 0;
    const uint_fast16_t lnum = 0;
};

struct Document
{
    //Document Nodes
    std::vector<_Lexeme> Lexemes;
    std::vector<struct Node> Nodes;
    std::vector<std::any> data;
};

auto tokenize(std::string &filedata, std::vector<_Lexeme> &Lexemes)
{
/*
Paste into regex101.com
Replace regex comments with R"()" by using \(\?#\s*\)

[rRfFUu]{0,2}?("{3}|")(?:[^"]|\\.|\\)*\1|(?#
)([rRfFUu]{0,2}?('{3}|')(?:[^']|\\.|\\)*\3)|(?#
)(#[^\r\n]*)|(?#
)([\n\r][ \t]*)|(?#
)(\\[^\r\n]*)|(?#
)((?#
    )(\.{3})|(?#
    )([0-9][0-9_]*\.[0-9_]*[0-9][0-9_]*[a-zA-Z]*)(?#
    )|([0-9][0-9_]*\.[a-zA-Z]*)|(?#
    )(\.[0-9][0-9_]*[a-zA-Z]*)|(?#
    )([<>*\/]{2})=?|(?#
    )([!%&*+\-<=>\/\\^|]=)(?#
))|(?#
)([!-\/:-@\[-^{-~]|[^\s!-\/:-@\[-^{-~]+)
*/
    //when rEgEX is A LaNGUAgE
    static std::regex TokenRegex(
                 R"([rRfFUu]{0,2}?("{3}|")(?:[^"]|\\.|\\)*\1|)"        //capture " strings
                R"(([rRfFUu]{0,2}?('{3}|')(?:[^']|\\.|\\)*\3)|)"       //capture ' strings
                R"((#[^\r\n]*)|)"                                      //capture comments
                R"(([\n\r][ \t]*)|)"                                   //capture newlines
                R"((\\[^\r\n]*)|)"                                     /* capture '\'andAnythingAfterTheBackslash  */
                R"(()"
                    R"((\.{3})|)"                                      //capture "..."
                    R"(([0-9][0-9_]*\.[0-9_]*[0-9][0-9_]*[a-zA-Z]*)|)" //──┬┬─> fucking floating point numbers
                    R"(([0-9][0-9_]*\.[a-zA-Z]*)|)"                    //──┘│
                    R"((\.[0-9][0-9_]*[a-zA-Z]*)|)"                    //───┘
                    R"(([<>*\/]{2})=?|)"                               //capture 2-3 character operators
                    R"(([!%&*+\-<=>\/\\^|]=))"                         //capture 2 caracter operators
                R"()|)"
                R"(([!-\/:-@\[-^{-~]|[^\s!-\/:-@\[-^{-~]+))"           //capture anything else
    );

    std::sregex_iterator rend, rit(filedata.begin(), filedata.end(), TokenRegex);
    Lexemes.reserve(std::distance(rit, rend));

    unsigned int ln = 0, nl_pos = 0;
    for (; rit != rend; ++rit){
        const auto& str = rit->str();
        if(str[0] == '\n' || str[0] == '\r'){
            ++ln;
            nl_pos = rit->position();
        }
        Lexemes.push_back({str, (uint_fast16_t)(rit->position() - nl_pos), ln});
    }
}

#if DEBUG_CPP
//for debugging
std::string getFlagName(turtle_flag flag)
{
    switch(flag >> tokenTypeOffset){
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
        auto & flag = tmpNode.NodeFlags;
        flag = turtle::token::flag::Control::NULL_TOKEN;

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
            flag |= turtle::token::flag::Control::NEWLINE | (len-1);
            }break;
        //fucking numbers
        case '0' ... '9':{
            switch(Lstr.size()){
                default:{
                enum {
                    RATIONAL_NUM,
                    EXPONENTIAL,
                    COMPLEX,
                    HEX_OR_OCTAL, //cuz they both have the same alphabet
                    BINARY
                };
                //parsing the numbers is too complex
                static std::regex regex [] = {
                    std::regex(R"(^([0-9]|_)+$)"),
                    std::regex(R"(^([0-9]|_)+[eE]([0-9]|_)+$)"),
                    std::regex(R"(^([0-9]|_)+[jJ]$)"),
                    std::regex(R"(^0[xXoO][0-9A-Fa-f_]+$)"),
                    std::regex(R"(^0[bB]([01]|_)+$)")
                };
                for(uint_fast8_t i = 0; i < sizeof(regex)/sizeof(regex[0]); ++i){
                    if(std::regex_match(Lstr, regex[i])){
                        switch(i){
                        case EXPONENTIAL:
                            panic("Exponetial numbers not supported cuz im not a nerd\n");
                            break;
                        case COMPLEX:
                            panic("Compex numbers not supported cuz I don't know what they are\n");
                            break;
                        case BINARY:
                            panic("Binary numbers not supported\n");
                            break;
                        case HEX_OR_OCTAL: break;
                        case RATIONAL_NUM:
                            //python outputs an error if your number is non zero but starts with zero
                            if(std::regex_match(Lstr, std::regex(R"(^[0_]+[_1-9]+$)"))){
                                panic("leading zeros in decimal integer literals are not permitted; "
                                      "use an 0o prefix for octal integers\n");
                            }
                            break;
                        }
                        goto _DATA_TYPE_NUMBER;
                    }
                }
                panic("Invalid number literal\n");
                }break;
                case 1:break;
            }
            _DATA_TYPE_NUMBER:
            std::string num('\0', Lstr.size());
            std::copy_if(Lstr.begin(), Lstr.end(),  std::back_inserter(num),
                [](auto c){
                    return c != '_';
                }
            );
            flag |= turtle::token::flag::Data::DATA_TYPE_NUMBER | Document.data.size();
            Document.data.push_back(
                        std::pair<std::string_view, turtle_int>(
                            {(std::string_view)Lstr, turtle_int(num)}
                        )
            );
        }break;
        //fucking strings
        case 'r':
        case 'R':
        case 'u':
        case 'U':
        case 'f'://f is for friendship :)
        case 'F':{
            if(Lstr.size() > 2){
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
                _NULL_FLAG     = 0b000,                                                                                      // │
                _RAW_FLAG      = 0b001,                                                                                      // │
                _UNICODE_FLAG  = 0b010,                                                                                      // │
                _FORMATED_FLAG = 0b100,                                                                                      // │
            };                                                                                                               // │
            //Generated via python program                                                                                   // │
            //Use this to tell if we've found a string prefix [rRuUfF]                                                       // │
            constexpr static void * pmap[] = {                                                                               // │
                &&_PUT_STRING_PREFIX, /*  0 */ &&_PUT_STRING_PREFIX, /*  1 */ &&_PUT_STRING_PREFIX, /*  2 */                 // │
                &&_PUT_STRING_PREFIX, /*  3 */ &&_PUT_STRING_PREFIX, /*  4 */ &&_PUT_STRING_PREFIX, /*  5 */                 // │
                &&_PUT_STRING_PREFIX, /*  6 */ &&_PUT_STRING_PREFIX, /*  7 */ &&_PUT_STRING_PREFIX, /*  8 */                 // │
                &&_PUT_STRING_PREFIX, /*  9 */ &&_PUT_STRING_PREFIX, /* 10 */ &&_PUT_STRING_PREFIX, /* 11 */                 // │
                &&_PUT_STRING_PREFIX, /* 12 */ &&_PUT_STRING_PREFIX, /* 13 */ &&_PUT_STRING_PREFIX, /* 14 */                 // │
                &&_PUT_STRING_PREFIX, /* 15 */ &&_PUT_STRING_PREFIX, /* 16 */ &&_PUT_STRING_PREFIX, /* 17 */                 // │
                &&_PUT_STRING_PREFIX, /* 18 */ &&_PUT_STRING_PREFIX, /* 19 */ &&_PUT_STRING_PREFIX, /* 20 */                 // │
                &&_PUT_STRING_PREFIX, /* 21 */ &&_PUT_STRING_PREFIX, /* 22 */ &&_PUT_STRING_PREFIX, /* 23 */                 // │
                &&_PUT_STRING_PREFIX, /* 24 */ &&_PUT_STRING_PREFIX, /* 25 */ &&_PUT_STRING_PREFIX, /* 26 */                 // │
                &&_PUT_STRING_PREFIX, /* 27 */ &&_PUT_STRING_PREFIX, /* 28 */ &&_PUT_STRING_PREFIX, /* 29 */                 // │
                &&_PUT_STRING_PREFIX, /* 30 */ &&_PUT_STRING_PREFIX, /* 31 */ &&_PUT_STRING_PREFIX, /* 32 */                 // │
                &&_PUT_STRING_PREFIX, /*  ! */ &&_PUT_STRING_PREFIX, /*  " */ &&_PUT_STRING_PREFIX, /*  # */                 // │
                &&_PUT_STRING_PREFIX, /*  $ */ &&_PUT_STRING_PREFIX, /*  % */ &&_PUT_STRING_PREFIX, /*  & */                 // │
                &&_PUT_STRING_PREFIX, /*  ' */ &&_PUT_STRING_PREFIX, /*  ( */ &&_PUT_STRING_PREFIX, /*  ) */                 // │
                &&_PUT_STRING_PREFIX, /*  * */ &&_PUT_STRING_PREFIX, /*  + */ &&_PUT_STRING_PREFIX, /*  , */                 // │
                &&_PUT_STRING_PREFIX, /*  - */ &&_PUT_STRING_PREFIX, /*  . */ &&_PUT_STRING_PREFIX, /*  / */                 // │
                &&_PUT_STRING_PREFIX, /*  0 */ &&_PUT_STRING_PREFIX, /*  1 */ &&_PUT_STRING_PREFIX, /*  2 */                 // │
                &&_PUT_STRING_PREFIX, /*  3 */ &&_PUT_STRING_PREFIX, /*  4 */ &&_PUT_STRING_PREFIX, /*  5 */                 // │
                &&_PUT_STRING_PREFIX, /*  6 */ &&_PUT_STRING_PREFIX, /*  7 */ &&_PUT_STRING_PREFIX, /*  8 */                 // │
                &&_PUT_STRING_PREFIX, /*  9 */ &&_PUT_STRING_PREFIX, /*  : */ &&_PUT_STRING_PREFIX, /*  ; */                 // │
                &&_PUT_STRING_PREFIX, /*  < */ &&_PUT_STRING_PREFIX, /*  = */ &&_PUT_STRING_PREFIX, /*  > */                 // │
                &&_PUT_STRING_PREFIX, /*  ? */ &&_PUT_STRING_PREFIX, /*  @ */ &&_PUT_STRING_PREFIX, /*  A */                 // │
                &&_PUT_STRING_PREFIX, /*  B */ &&_PUT_STRING_PREFIX, /*  C */ &&_PUT_STRING_PREFIX, /*  D */                 // │
                &&_PUT_STRING_PREFIX, /*  E */ &&_GET_STRING_PREFIX, /*  F */ &&_PUT_STRING_PREFIX, /*  G */                 // │
                &&_PUT_STRING_PREFIX, /*  H */ &&_PUT_STRING_PREFIX, /*  I */ &&_PUT_STRING_PREFIX, /*  J */                 // │
                &&_PUT_STRING_PREFIX, /*  K */ &&_PUT_STRING_PREFIX, /*  L */ &&_PUT_STRING_PREFIX, /*  M */                 // │
                &&_PUT_STRING_PREFIX, /*  N */ &&_PUT_STRING_PREFIX, /*  O */ &&_PUT_STRING_PREFIX, /*  P */                 // │
                &&_PUT_STRING_PREFIX, /*  Q */ &&_GET_STRING_PREFIX, /*  R */ &&_PUT_STRING_PREFIX, /*  S */                 // │
                &&_PUT_STRING_PREFIX, /*  T */ &&_GET_STRING_PREFIX, /*  U */ &&_PUT_STRING_PREFIX, /*  V */                 // │
                &&_PUT_STRING_PREFIX, /*  W */ &&_PUT_STRING_PREFIX, /*  X */ &&_PUT_STRING_PREFIX, /*  Y */                 // │
                &&_PUT_STRING_PREFIX, /*  Z */ &&_PUT_STRING_PREFIX, /*  [ */ &&_PUT_STRING_PREFIX, /*  \ */                 // │
                &&_PUT_STRING_PREFIX, /*  ] */ &&_PUT_STRING_PREFIX, /*  ^ */ &&_PUT_STRING_PREFIX, /*  _ */                 // │
                &&_PUT_STRING_PREFIX, /*  ` */ &&_PUT_STRING_PREFIX, /*  a */ &&_PUT_STRING_PREFIX, /*  b */                 // │
                &&_PUT_STRING_PREFIX, /*  c */ &&_PUT_STRING_PREFIX, /*  d */ &&_PUT_STRING_PREFIX, /*  e */                 // │
                &&_GET_STRING_PREFIX, /*  f */ &&_PUT_STRING_PREFIX, /*  g */ &&_PUT_STRING_PREFIX, /*  h */                 // │
                &&_PUT_STRING_PREFIX, /*  i */ &&_PUT_STRING_PREFIX, /*  j */ &&_PUT_STRING_PREFIX, /*  k */                 // │
                &&_PUT_STRING_PREFIX, /*  l */ &&_PUT_STRING_PREFIX, /*  m */ &&_PUT_STRING_PREFIX, /*  n */                 // │
                &&_PUT_STRING_PREFIX, /*  o */ &&_PUT_STRING_PREFIX, /*  p */ &&_PUT_STRING_PREFIX, /*  q */                 // │
                &&_GET_STRING_PREFIX, /*  r */ &&_PUT_STRING_PREFIX, /*  s */ &&_PUT_STRING_PREFIX, /*  t */                 // │
                &&_GET_STRING_PREFIX, /*  u */ &&_PUT_STRING_PREFIX, /*  v */ &&_PUT_STRING_PREFIX, /*  w */                 // │
                &&_PUT_STRING_PREFIX, /*  x */ &&_PUT_STRING_PREFIX, /*  y */ &&_PUT_STRING_PREFIX, /*  z */                 // │
                &&_PUT_STRING_PREFIX, /*  { */ &&_PUT_STRING_PREFIX, /*  | */ &&_PUT_STRING_PREFIX, /*  } */                 // │
                &&_PUT_STRING_PREFIX, /*  ~ */                                                                               // │
            };                                                                                                               // │
                                                                                                                             // └>───────────────────────────────>┐
            /*                                                                                                                                                 // │
             * if only we could do                                                                                                                             // │
             *                                                                                                                                                 // │
             * static const uint_fast8_t fmap[127] = {
             *                  _NULL_FLAG,       //zero initialize any unspecified elements
             *      ['R'] =      _RAW_FLAG,
             *      ['r'] =      _RAW_FLAG,
             *      ['f'] = _FORMATED_FLAG,
             *      ['F'] = _FORMATED_FLAG,
             *      ['U'] =  _UNICODE_FLAG,
             *      ['u'] =  _UNICODE_FLAG
             * };
             *
             * like in C 😭
             * That would make constructing a jumptable so much simpler
            */
            constexpr static uint_fast8_t fmap[] = {
                  _NULL_FLAG, /*  0 */     _NULL_FLAG, /*  1 */     _NULL_FLAG, /*  2 */     _NULL_FLAG, /*  3 */     _NULL_FLAG, /*  4 */
                  _NULL_FLAG, /*  5 */     _NULL_FLAG, /*  6 */     _NULL_FLAG, /*  7 */     _NULL_FLAG, /*  8 */     _NULL_FLAG, /*  9 */
                  _NULL_FLAG, /* 10 */     _NULL_FLAG, /* 11 */     _NULL_FLAG, /* 12 */     _NULL_FLAG, /* 13 */     _NULL_FLAG, /* 14 */
                  _NULL_FLAG, /* 15 */     _NULL_FLAG, /* 16 */     _NULL_FLAG, /* 17 */     _NULL_FLAG, /* 18 */     _NULL_FLAG, /* 19 */
                  _NULL_FLAG, /* 20 */     _NULL_FLAG, /* 21 */     _NULL_FLAG, /* 22 */     _NULL_FLAG, /* 23 */     _NULL_FLAG, /* 24 */
                  _NULL_FLAG, /* 25 */     _NULL_FLAG, /* 26 */     _NULL_FLAG, /* 27 */     _NULL_FLAG, /* 28 */     _NULL_FLAG, /* 29 */
                  _NULL_FLAG, /* 30 */     _NULL_FLAG, /* 31 */     _NULL_FLAG, /* 32 */     _NULL_FLAG, /*  ! */     _NULL_FLAG, /*  " */
                  _NULL_FLAG, /*  # */     _NULL_FLAG, /*  $ */     _NULL_FLAG, /*  % */     _NULL_FLAG, /*  & */     _NULL_FLAG, /*  ' */
                  _NULL_FLAG, /*  ( */     _NULL_FLAG, /*  ) */     _NULL_FLAG, /*  * */     _NULL_FLAG, /*  + */     _NULL_FLAG, /*  , */
                  _NULL_FLAG, /*  - */     _NULL_FLAG, /*  . */     _NULL_FLAG, /*  / */     _NULL_FLAG, /*  0 */     _NULL_FLAG, /*  1 */
                  _NULL_FLAG, /*  2 */     _NULL_FLAG, /*  3 */     _NULL_FLAG, /*  4 */     _NULL_FLAG, /*  5 */     _NULL_FLAG, /*  6 */
                  _NULL_FLAG, /*  7 */     _NULL_FLAG, /*  8 */     _NULL_FLAG, /*  9 */     _NULL_FLAG, /*  : */     _NULL_FLAG, /*  ; */
                  _NULL_FLAG, /*  < */     _NULL_FLAG, /*  = */     _NULL_FLAG, /*  > */     _NULL_FLAG, /*  ? */     _NULL_FLAG, /*  @ */
                  _NULL_FLAG, /*  A */     _NULL_FLAG, /*  B */     _NULL_FLAG, /*  C */     _NULL_FLAG, /*  D */     _NULL_FLAG, /*  E */
              _FORMATED_FLAG, /*  F */     _NULL_FLAG, /*  G */     _NULL_FLAG, /*  H */     _NULL_FLAG, /*  I */     _NULL_FLAG, /*  J */
                  _NULL_FLAG, /*  K */     _NULL_FLAG, /*  L */     _NULL_FLAG, /*  M */     _NULL_FLAG, /*  N */     _NULL_FLAG, /*  O */
                  _NULL_FLAG, /*  P */     _NULL_FLAG, /*  Q */      _RAW_FLAG, /*  R */     _NULL_FLAG, /*  S */     _NULL_FLAG, /*  T */
               _UNICODE_FLAG, /*  U */     _NULL_FLAG, /*  V */     _NULL_FLAG, /*  W */     _NULL_FLAG, /*  X */     _NULL_FLAG, /*  Y */
                  _NULL_FLAG, /*  Z */     _NULL_FLAG, /*  [ */     _NULL_FLAG, /*  \ */     _NULL_FLAG, /*  ] */     _NULL_FLAG, /*  ^ */
                  _NULL_FLAG, /*  _ */     _NULL_FLAG, /*  ` */     _NULL_FLAG, /*  a */     _NULL_FLAG, /*  b */     _NULL_FLAG, /*  c */
                  _NULL_FLAG, /*  d */     _NULL_FLAG, /*  e */ _FORMATED_FLAG, /*  f */     _NULL_FLAG, /*  g */     _NULL_FLAG, /*  h */
                  _NULL_FLAG, /*  i */     _NULL_FLAG, /*  j */     _NULL_FLAG, /*  k */     _NULL_FLAG, /*  l */     _NULL_FLAG, /*  m */
                  _NULL_FLAG, /*  n */     _NULL_FLAG, /*  o */     _NULL_FLAG, /*  p */     _NULL_FLAG, /*  q */      _RAW_FLAG, /*  r */
                  _NULL_FLAG, /*  s */     _NULL_FLAG, /*  t */  _UNICODE_FLAG, /*  u */     _NULL_FLAG, /*  v */     _NULL_FLAG, /*  w */
                  _NULL_FLAG, /*  x */     _NULL_FLAG, /*  y */     _NULL_FLAG, /*  z */     _NULL_FLAG, /*  { */     _NULL_FLAG, /*  | */
                  _NULL_FLAG, /*  } */     _NULL_FLAG, /*  ~ */                                                                                                // │
            };                                                                                                                                                 // │
                                                                                                                                                               // │
            /*                                                                                                                                                 // │
             * //The below is the equivelent code                                                                                                              // │
             * for(auto * c = Lstr.data(); !(*c == '"' || *c == '\''); ++c){                                                                                   // │
             *      f ^= fmap[*c];                                                                                                                             // │
             *      if(f == _NULL_FLAG){                                                                                                                       // │
             *          exit(1);                                                                                                                               // │
             *      }                                                                                                                                          // │
             * }                                                                                                                                               // │
             *                                                                                                                                                 // │
             */                                                                                                                                                // │
            //sexy, instantaneous                                                                                                                              // │
            uint_fast8_t f = 0;                                                                                                                                // │
            auto * c = Lstr.data();                                                                                                                            // │
            //takes around 12 clock cycles to transform the string prefix into a flag                                                                          // │
            _GET_STRING_PREFIX:{                                                                                                                               // │
                // ┌───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────<┐              // │
                // ↓                                                                                                                         // │              // │
                    //assign flag                                                                                                            // │              // │
                    //cast c to uint_fast8_t in order to avoid a warning                                                                     // │              // │
                    f ^= *(fmap + *c);                                                                                                       // │              // │
                                                                                                                                             // │              // │
                    /*                                                                                                                       // │              // │
                     * the python interpreter throws an error if the prefix is specified twice, so we will too :(                            // │              // │
                     * if someone for what ever reason wanted this not to thow and error and have a more flexable experience,                // │              // │
                     * they could change the XOR above to an OR, and the compiler would still work ;)                                        // │              // │
                     */                                                                                                                      // │              // │
                                                                                                                                             // │              // │
                                                                                                                                             // │              // │
               /*                                                                                                                            // │              // │
                * break out of the loop if(c == '"' or c == '\'') else goto _GET_STRING_PREFIX;                                              // │              // │
                *                                                                                                                            // │              // │
                *  - unconditional jump can be predicted by the cpu                                                                          // │              // │
                *  - using this method instead of a regualar for loop has certian advantages                                                 // │              // │
                *    * no temporary "i" variable to increment, we're only incrementing the string pointer                                    // │              // │
                *    * no comparison of max, i.e for(i < max; continue){}                                                                    // │              // │
                *                                                                                                                            // │              // │
                * cast var c to uint_fast8_t in order to avoid warning                                                                       // │              // │
                *                                                                                                                            // │              // │
                * note to future self:                                                                                                       // │              // │
                *   pmap stands for prefix map                                                                                               // │              // │
                */                                                                                                                           // │              // │
                                                              // ++c;                                                                        // │              // │
                goto **(pmap + *++c);                         //if( (*c == '"' || *c == '\"' ) == false ){                                   // │              // │
                                                              //    goto _GET_STRING_PREFIX;                // ────────────────────────────────>┘              // │
                                                              //}                                                                                              // │
                                                              //else{                                                                                          // │
                                                              //    goto _PUT_STRING_PREFIX;                // ┐                                               // │
                                                              //}                                           // │                                               // │
            }                                                                                               // │                                               // │
                                                                                                            // │                                               // │
            _PUT_STRING_PREFIX:  // <─────────────────────────────────────────────────────────────────────────<┘                                               // │
            constexpr static turtle_flag tmap[]={
                0,                                                            // 0 0b000 _NULL_FLAG                   // error: string prefix specified twice
                turtle::token::flag::Data::DATA_TYPE_RAW_STRING,              // 1 0b001 _RAW_FLAG
                turtle::token::flag::Data::DATA_TYPE_UNICODE_STRING,          // 2 0b010 _UNICODE_FLAG
                0,                                                            // 3 0b011 _RAW_FLAG      |  _UNICODE_FLAG //error: no such thing as a rU string
                turtle::token::flag::Data::DATA_TYPE_FORMATED_STRING,         // 4 0b100 _FORMATED_FLAG
                0,                                                            // 5 0b101 _RAW_FLAG      | _FORMATED_FLAG //error: no such thing as a rF string
                turtle::token::flag::Data::DATA_TYPE_FORMATED_UNICODE_STRING  // 6 0b110 _UNICODE_FLAG  | _FORMATED_FLAG
            };
            flag |= tmap[f];
            if(flag == turtle::token::flag::Control::NULL_TOKEN){
                constexpr uint_fast8_t preStrs[7] = {0,0,0,1};//lazy
                constexpr const char * pres[] =  {"formatted", "unicode"};
                switch(f){
                    case _RAW_FLAG |  _UNICODE_FLAG:
                    case _RAW_FLAG | _FORMATED_FLAG:
                        panic("Line %d:%d Theres no such thing as a raw %s string\n", Lexeme.lnum + 1, Lexeme.lpos, pres[preStrs[f]]);
                        break;
                    case 0:
                        panic("Line %d:%d String prefix specified twice\n", Lexeme.lnum + 1, Lexeme.lpos, pres[preStrs[f]]);
                        break;
                }
            }
                                                                                                                                                               // │
        }                                                                                                                                                      // │
        [[fallthrough]];                                                                                                                                       // │
        case '"':                                                                                                                                              // │
        case '\'':{                                                                                                                                            // │
            if(Lstr.length() == 1){                                                                                                                            // │
                panic("Line %d:%d Non terminating string %c\n", Lexeme.lnum + 1, Lexeme.lpos, Lstr[0]);                                                        // │
            }                                                                                                                                                  // │
            flag |= turtle::token::flag::Data::DATA_TYPE_STRING | Document.data.size();                                                                        // │
            Document.data.push_back((std::string_view)Lstr);                                                                                                   // │
            }break;                                                                                                                                            // │
        default:                                                                                                                                               // │
            _DEFUALT_FIND_TOKEN: // <────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────<┘
            //turtle::findToken returns 0 if not found in the predefined tokens map
            if( (flag |= turtle::findToken(Lstr.c_str())) ){
                break;
            }
            flag |= turtle::token::flag::Type::IDENTIFIER | Document.data.size();
            Document.data.push_back(Lstr);
            break;
        }

#if DEBUG_CPP

        std::string res = Lstr;
        if(res[0] == '\n' || res[0] == '\r'){
            for(auto& ex : character_substitutions){
                res = std::regex_replace(res, std::regex(ex[0]), ex[1]);
            }
        }
        std::ostringstream snake_y;
        snake_y << std::bitset< sizeof(turtle_flag)*8 >(tmpNode.NodeFlags);
        const std::string& flag_str = getFlagName(tmpNode.NodeFlags);
        std::string ws; //                        The token with the largest name is 32 characters long
        for(int i=0, size = flag_str.size(); i < (33 - size); ++i){ws+=' ';}

        snake_y << " | Predicted token -> " << flag_str << ws
           << " | token -> [";

        //if string, format it so stays on the right margin instead of wraping in the console preview
        if(res[0] == '"' || res[0] == '\''){
            static std::regex regex(R"(\n)");
            std::vector<std::string> lines(
                std::sregex_token_iterator(res.begin(), res.end(), regex, -1),
                {}
            );
            for(uint_fast8_t i = 0; i < (uint_fast8_t)lines.size(); ++i){
                if(i > 0){snake_y << '\n';}
                snake_y << lines[i];
            }
        } else {
            snake_y << res;
        }
        //oh shit its a snakeeeee
        std::cout << snake_y.str() << "]\n";

#endif // ifdef DEBUG

        tmpNode.line = std::move(Lexeme.lnum);
        tmpNode.linepos = std::move(Lexeme.lpos);
        Document.Nodes.push_back(tmpNode);
    }
}

} // namespace turtle

#endif // MAIN_H
