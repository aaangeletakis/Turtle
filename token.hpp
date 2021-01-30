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
    std::vector<struct Node> Nodes;
    std::vector<_Lexeme> Lexemes;
    std::vector<std::any> data;
};

auto tokenize(std::string &filedata, std::vector<_Lexeme> &Lexemes)
{
#include "regex_macros_def.h"
/*
[rRfFUu]{0,2}?"{3}(?:[^\\"]|\\.)*"{3}
|[rRfFUu]{0,2}?'{3}(?:[^\\']|\\.)*'{3}
|([rRfFUu]{0,2}?'(?:[^\\']|\\.)*'|[rRfFUu]{0,2}?"(?:[^\\"]|\\.)*")
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

#ifdef DEBUG
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

#ifdef DEBUG
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
                        len -= 1;
                        len += 8;
                    }
                }
            }
            flag |= turtle::token::flag::Control::NEWLINE | (len-1);
            }break;
        //fucking numbers
        case '0' ... '9':{
            switch(Lstr.size()){
                default:{
                enum{
                    INVALID_CHARACTER        = 0b000,
                    VALID_CHARACTER          = 0b010,
                    POSTFIX                  = 0b001,
                    IS_ZERO                  = 0b110,

                    EXPONENT                  =    0,
                    HEX,
                    BINARY,
                    OCTAL,
                    COMPLEX,
                    STARTS_W_ZERO,
                    HAS_POSTFIX,


#define numberFlag_M(F) (F << 2)
                    EXPONENT_FLAG = numberFlag_M(EXPONENT) | POSTFIX,
                    HEX_FLAG =      numberFlag_M(HEX)      | POSTFIX,
                    BINARY_FLAG =   numberFlag_M(BINARY)   | POSTFIX,
                    OCTAL_FLAG =    numberFlag_M(OCTAL)    | POSTFIX,
                    COMPLEX_FLAG =  numberFlag_M(COMPLEX)  | POSTFIX,
#undef numberFlag_M

                    EXPONENT_BIT      = 1 << EXPONENT,
                    HEX_BIT           = 1 << HEX,
                    BINARY_BIT        = 1 << BINARY,
                    OCTAL_BIT         = 1 << OCTAL,
                    COMPLEX_BIT       = 1 << COMPLEX,
                    STARTS_W_ZERO_BIT = 1 << STARTS_W_ZERO,
                    HAS_POSTFIX_BIT   = 1 << HAS_POSTFIX,
                    REGUALR_NUMBER = 0,

                };

                constexpr static uint_fast8_t char_map[]={
                         INVALID_CHARACTER, /*  0 */      INVALID_CHARACTER, /*  1 */      INVALID_CHARACTER, /*  2 */      INVALID_CHARACTER, /*  3 */
                         INVALID_CHARACTER, /*  4 */      INVALID_CHARACTER, /*  5 */      INVALID_CHARACTER, /*  6 */      INVALID_CHARACTER, /*  7 */
                         INVALID_CHARACTER, /*  8 */      INVALID_CHARACTER, /*  9 */      INVALID_CHARACTER, /* 10 */      INVALID_CHARACTER, /* 11 */
                         INVALID_CHARACTER, /* 12 */      INVALID_CHARACTER, /* 13 */      INVALID_CHARACTER, /* 14 */      INVALID_CHARACTER, /* 15 */
                         INVALID_CHARACTER, /* 16 */      INVALID_CHARACTER, /* 17 */      INVALID_CHARACTER, /* 18 */      INVALID_CHARACTER, /* 19 */
                         INVALID_CHARACTER, /* 20 */      INVALID_CHARACTER, /* 21 */      INVALID_CHARACTER, /* 22 */      INVALID_CHARACTER, /* 23 */
                         INVALID_CHARACTER, /* 24 */      INVALID_CHARACTER, /* 25 */      INVALID_CHARACTER, /* 26 */      INVALID_CHARACTER, /* 27 */
                         INVALID_CHARACTER, /* 28 */      INVALID_CHARACTER, /* 29 */      INVALID_CHARACTER, /* 30 */      INVALID_CHARACTER, /* 31 */
                         INVALID_CHARACTER, /* 32 */      INVALID_CHARACTER, /*  ! */      INVALID_CHARACTER, /*  " */      INVALID_CHARACTER, /*  # */
                         INVALID_CHARACTER, /*  $ */      INVALID_CHARACTER, /*  % */      INVALID_CHARACTER, /*  & */      INVALID_CHARACTER, /*  ' */
                         INVALID_CHARACTER, /*  ( */      INVALID_CHARACTER, /*  ) */      INVALID_CHARACTER, /*  * */      INVALID_CHARACTER, /*  + */
                         INVALID_CHARACTER, /*  , */      INVALID_CHARACTER, /*  - */      INVALID_CHARACTER, /*  . */      INVALID_CHARACTER, /*  / */

                /* you've just gotta be
                 * special don't you zero*/
                                   IS_ZERO, /*  0 */        VALID_CHARACTER, /*  1 */        VALID_CHARACTER, /*  2 */        VALID_CHARACTER, /*  3 */
                           VALID_CHARACTER, /*  4 */        VALID_CHARACTER, /*  5 */        VALID_CHARACTER, /*  6 */        VALID_CHARACTER, /*  7 */
                           VALID_CHARACTER, /*  8 */        VALID_CHARACTER, /*  9 */      INVALID_CHARACTER, /*  : */      INVALID_CHARACTER, /*  ; */
                         INVALID_CHARACTER, /*  < */      INVALID_CHARACTER, /*  = */      INVALID_CHARACTER, /*  > */      INVALID_CHARACTER, /*  ? */
                         INVALID_CHARACTER, /*  @ */      INVALID_CHARACTER, /*  A */      INVALID_CHARACTER, /*  B */      INVALID_CHARACTER, /*  C */
                         INVALID_CHARACTER, /*  D */      INVALID_CHARACTER, /*  E */      INVALID_CHARACTER, /*  F */      INVALID_CHARACTER, /*  G */
                         INVALID_CHARACTER, /*  H */      INVALID_CHARACTER, /*  I */      INVALID_CHARACTER, /*  J */      INVALID_CHARACTER, /*  K */
                         INVALID_CHARACTER, /*  L */      INVALID_CHARACTER, /*  M */      INVALID_CHARACTER, /*  N */      INVALID_CHARACTER, /*  O */
                         INVALID_CHARACTER, /*  P */      INVALID_CHARACTER, /*  Q */      INVALID_CHARACTER, /*  R */      INVALID_CHARACTER, /*  S */
                         INVALID_CHARACTER, /*  T */      INVALID_CHARACTER, /*  U */      INVALID_CHARACTER, /*  V */      INVALID_CHARACTER, /*  W */
                         INVALID_CHARACTER, /*  X */      INVALID_CHARACTER, /*  Y */      INVALID_CHARACTER, /*  Z */      INVALID_CHARACTER, /*  [ */
                         INVALID_CHARACTER, /*  \ */      INVALID_CHARACTER, /*  ] */      INVALID_CHARACTER, /*  ^ */        VALID_CHARACTER, /*  _ */
                         INVALID_CHARACTER, /*  ` */      INVALID_CHARACTER, /*  a */            BINARY_FLAG, /*  b */      INVALID_CHARACTER, /*  c */
                         INVALID_CHARACTER, /*  d */          EXPONENT_FLAG, /*  e */      INVALID_CHARACTER, /*  f */      INVALID_CHARACTER, /*  g */
                         INVALID_CHARACTER, /*  h */      INVALID_CHARACTER, /*  i */           COMPLEX_FLAG, /*  j */      INVALID_CHARACTER, /*  k */
                         INVALID_CHARACTER, /*  l */      INVALID_CHARACTER, /*  m */      INVALID_CHARACTER, /*  n */             OCTAL_FLAG, /*  o */
                         INVALID_CHARACTER, /*  p */      INVALID_CHARACTER, /*  q */      INVALID_CHARACTER, /*  r */      INVALID_CHARACTER, /*  s */
                         INVALID_CHARACTER, /*  t */      INVALID_CHARACTER, /*  u */      INVALID_CHARACTER, /*  v */      INVALID_CHARACTER, /*  w */
                                  HEX_FLAG, /*  x */      INVALID_CHARACTER, /*  y */      INVALID_CHARACTER, /*  z */      INVALID_CHARACTER, /*  { */
                         INVALID_CHARACTER, /*  | */      INVALID_CHARACTER, /*  } */      INVALID_CHARACTER, /*  ~ */
                };
                /*
                 * f -> 0b0000_0000
                 *        ││└─────┴> Number postfix indicator
                 *        │└───────> Starts with zero
                 *        └────────> Persistant bit to tell if postfix has already been set
                 */
                auto * c = Lstr.data();
                uint_fast8_t f = 0;
                const auto len = Lstr.length() - 1;
                for(uint_fast8_t i = 0; i < len; ++i, ++c){
                    const auto char_flag = char_map[(uint_fast8_t)*c];
                    if(char_flag == INVALID_CHARACTER){
                        panic("Line %d:%d Invalid postfix for number\n", Lexeme.lnum + 1, Lexeme.lpos);
                    }
                    if(char_flag & 1){
                        if(f & (HAS_POSTFIX)){
                            panic("Number prefix set twice\n");
                        } else {
                            f |= (1 << (char_flag >> 2));
                            f |= HAS_POSTFIX;
                        }
                    }
                }
                switch(f){
                case EXPONENT_BIT:
                    panic("Exponet literals not supported\n");
                    break;
                case HEX_BIT:
                case BINARY_BIT:
                case OCTAL_BIT:
                    panic("Octal literals not supported as I don't use them\n");
                    break;
                case COMPLEX_BIT:
                    panic("Complex literals not supported as I don't know what they are\n");
                    break;
                case REGUALR_NUMBER:
                    break;
                }
                }break;
                case 1:break;
            }
            flag |= turtle::token::flag::Data::DATA_TYPE_NUMBER | Document.data.size();
            Document.data.push_back((std::string_view)Lstr);
        }break;
        //fucking strings
        case 'r':
        case 'R':
        case 'u':
        case 'U':
        case 'f':
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
                _RAW_FLAG      = 0b001,                                                                                      // │
                _UNICODE_FLAG  = 0b010,                                                                                      // │
                _FORMATED_FLAG = 0b100,                                                                                      // │
            };                                                                                                               // │
            //Generated via python program                                                                                   // │
            //Use this to tell if we've found a string prefix                                                                // │
            constexpr static uint_fast8_t pmap[] = {                                                                         // │
                0, /*  0 */ 0, /*  1 */ 0, /*  2 */ 0, /*  3 */ 0, /*  4 */ 0, /*  5 */ 0, /*  6 */ 0, /*  7 */ 0, /*  8 */  // │
                0, /*  9 */ 0, /* 10 */ 0, /* 11 */ 0, /* 12 */ 0, /* 13 */ 0, /* 14 */ 0, /* 15 */ 0, /* 16 */ 0, /* 17 */  // │
                0, /* 18 */ 0, /* 19 */ 0, /* 20 */ 0, /* 21 */ 0, /* 22 */ 0, /* 23 */ 0, /* 24 */ 0, /* 25 */ 0, /* 26 */  // │
                0, /* 27 */ 0, /* 28 */ 0, /* 29 */ 0, /* 30 */ 0, /* 31 */ 0, /* 32 */ 0, /*  ! */ 0, /*  " */ 0, /*  # */  // │
                0, /*  $ */ 0, /*  % */ 0, /*  & */ 0, /*  ' */ 0, /*  ( */ 0, /*  ) */ 0, /*  * */ 0, /*  + */ 0, /*  , */  // │
                0, /*  - */ 0, /*  . */ 0, /*  / */ 0, /*  0 */ 0, /*  1 */ 0, /*  2 */ 0, /*  3 */ 0, /*  4 */ 0, /*  5 */  // │
                0, /*  6 */ 0, /*  7 */ 0, /*  8 */ 0, /*  9 */ 0, /*  : */ 0, /*  ; */ 0, /*  < */ 0, /*  = */ 0, /*  > */  // │
                0, /*  ? */ 0, /*  @ */ 0, /*  A */ 0, /*  B */ 0, /*  C */ 0, /*  D */ 0, /*  E */ 1, /*  F */ 0, /*  G */  // │
                0, /*  H */ 0, /*  I */ 0, /*  J */ 0, /*  K */ 0, /*  L */ 0, /*  M */ 0, /*  N */ 0, /*  O */ 0, /*  P */  // │
                0, /*  Q */ 1, /*  R */ 0, /*  S */ 0, /*  T */ 1, /*  U */ 0, /*  V */ 0, /*  W */ 0, /*  X */ 0, /*  Y */  // │
                0, /*  Z */ 0, /*  [ */ 0, /*  \ */ 0, /*  ] */ 0, /*  ^ */ 0, /*  _ */ 0, /*  ` */ 0, /*  a */ 0, /*  b */  // │
                0, /*  c */ 0, /*  d */ 0, /*  e */ 1, /*  f */ 0, /*  g */ 0, /*  h */ 0, /*  i */ 0, /*  j */ 0, /*  k */  // │
                0, /*  l */ 0, /*  m */ 0, /*  n */ 0, /*  o */ 0, /*  p */ 0, /*  q */ 1, /*  r */ 0, /*  s */ 0, /*  t */  // │
                1, /*  u */ 0, /*  v */ 0, /*  w */ 0, /*  x */ 0, /*  y */ 0, /*  z */ 0, /*  { */ 0, /*  | */ 0, /*  } */  // │
                0, /*  ~ */                                                                                                  // │
            };                                                                                                               // │
                                                                                                                             // └>───────────────────────────────>┐
            constexpr static uint_fast8_t fmap[] = {                                                                         //                                   │
                0, /*  0 */               0, /*  1 */               0, /*  2 */               0, /*  3 */               0, /*  4 */               0, /*  5 */  // │
                0, /*  6 */               0, /*  7 */               0, /*  8 */               0, /*  9 */               0, /* 10 */               0, /* 11 */  // │
                0, /* 12 */               0, /* 13 */               0, /* 14 */               0, /* 15 */               0, /* 16 */               0, /* 17 */  // │
                0, /* 18 */               0, /* 19 */               0, /* 20 */               0, /* 21 */               0, /* 22 */               0, /* 23 */  // │
                0, /* 24 */               0, /* 25 */               0, /* 26 */               0, /* 27 */               0, /* 28 */               0, /* 29 */  // │
                0, /* 30 */               0, /* 31 */               0, /* 32 */               0, /*  ! */               0, /*  " */               0, /*  # */  // │
                0, /*  $ */               0, /*  % */               0, /*  & */               0, /*  ' */               0, /*  ( */               0, /*  ) */  // │
                0, /*  * */               0, /*  + */               0, /*  , */               0, /*  - */               0, /*  . */               0, /*  / */  // │
                0, /*  0 */               0, /*  1 */               0, /*  2 */               0, /*  3 */               0, /*  4 */               0, /*  5 */  // │
                0, /*  6 */               0, /*  7 */               0, /*  8 */               0, /*  9 */               0, /*  : */               0, /*  ; */  // │
                0, /*  < */               0, /*  = */               0, /*  > */               0, /*  ? */               0, /*  @ */               0, /*  A */  // │
                0, /*  B */               0, /*  C */               0, /*  D */               0, /*  E */  _FORMATED_FLAG, /*  F */               0, /*  G */  // │
                0, /*  H */               0, /*  I */               0, /*  J */               0, /*  K */               0, /*  L */               0, /*  M */  // │
                0, /*  N */               0, /*  O */               0, /*  P */               0, /*  Q */       _RAW_FLAG, /*  R */               0, /*  S */  // │
                0, /*  T */   _UNICODE_FLAG, /*  U */               0, /*  V */               0, /*  W */               0, /*  X */               0, /*  Y */  // │
                0, /*  Z */               0, /*  [ */               0, /*  \ */               0, /*  ] */               0, /*  ^ */               0, /*  _ */  // │
                0, /*  ` */               0, /*  a */               0, /*  b */               0, /*  c */               0, /*  d */               0, /*  e */  // │
   _FORMATED_FLAG, /*  f */               0, /*  g */               0, /*  h */               0, /*  i */               0, /*  j */               0, /*  k */  // │
                0, /*  l */               0, /*  m */               0, /*  n */               0, /*  o */               0, /*  p */               0, /*  q */  // │
        _RAW_FLAG, /*  r */               0, /*  s */               0, /*  t */   _UNICODE_FLAG, /*  u */               0, /*  v */               0, /*  w */  // │
                0, /*  x */               0, /*  y */               0, /*  z */               0, /*  { */               0, /*  | */               0, /*  } */  // │
                0, /*  ~ */                                                                                                                                    // │
            };                                                                                                                                                 // │
            constexpr static void *prefix[] = { &&_ASSIGN_STRING_PREFIX, &&_GET_STRING_PREFIX };                                                               // │
            //sexy, instantaneous, O(~20) worst possible outcome to get string prefix                                                                          // │
            uint_fast8_t f = 0;                                                                                                                                // │
            auto * c = Lstr.data();                                                                                                                            // │
            for(uint_fast8_t j=0; j < 3; ++j, ++c){                                                                                                            // │
                                                                                                                                                               // │
                /*                                                                                                                                             // │
                 * break out of the loop if " or ' else continue                                                                                               // │
                 *  - unconditional jump can be predicted by the cpu                                                                                           // │
                 * cast var c to uint_fast8_t in order to avoid warning                                                                                        // │
                 */                                                                                                                                            // │
                goto* prefix[                                      //if (c == string prefix){ goto _GET_STRING_PREFIX; }     ───>┐                             // │
                                                                   //else                   { goto _ASSIGN_STRING_PREFIX; }  ────┼────────────>┐               // │
                        pmap[static_cast<uint_fast8_t>(*c)]        //                                                            │             │               // │
                ];                                                 //                                                            │             │               // │
                _GET_STRING_PREFIX:                                // <─────────────────────────────────────────────────────────<┘             │               // │
                                                                                                                                            // │               // │
                //assign flag                                                                                                               // │               // │
                //cast var c to uint_fast8_t in order to avoid warning                                                                      // │               // │
                f ^= fmap[static_cast<uint_fast8_t>(*c)];                                                                                   // │               // │
                                                                                                                                            // │               // │
                /*                                                                                                                          // │               // │
                 * the python interpreter throws an error if the prefix is specified twice, so we will too :(                               // │               // │
                 * if someone for what ever reason wanted this not to thow and error and a more flexable experience,                        // │               // │
                 * they could remove this, change the XOR above to an OR, and the compiler would still work ;)                              // │               // │
                 */                                                                                                                         // │               // │
                if(f == 0){                                                                                                                 // │               // │
                    panic("Line %d:%d String prefix specified twice\n", Lexeme.lnum + 1, Lexeme.lpos);                                      // │               // │
                }                                                                                                                           // │               // │
            }                                                                                                                               // │               // │
            /*                                                                                                                              // │               // │
             * the below panic should never be executed, as when the above loop                                                             // │               // │
             * finds a " or ' it will jump to _ASSIGN_STRING_PREFIX                                                                         // │               // │
             * but just in case, it's here                                                                                                  // │               // │
             */                                                                                                                             // │               // │
            panic("Something went wrong at line %d in string prefix section\n"                                                              // │               // │
                  "token -> [%s]\n", __LINE__, Lstr.c_str());                                                                               // │               // │
                                                                                                                                            // │               // │
            _ASSIGN_STRING_PREFIX:                                 // <───────────────────────────────────────────────────────────────────────<┘               // │
            switch(f){                                                                                                                                         // │
            case _RAW_FLAG     | _FORMATED_FLAG:{                                                                                                              // │
                panic("Line %d:%d There is no such thing as a raw formatted string!\n", Lexeme.lnum + 1, Lexeme.lpos);                                         // │
                }break;                                                                                                                                        // │
            case _RAW_FLAG     | _UNICODE_FLAG:{                                                                                                               // │
                panic("Line %d:%d There is no such thing as a raw unicode string!\n", Lexeme.lnum + 1, Lexeme.lpos);                                           // │
                }break;                                                                                                                                        // │
            case _UNICODE_FLAG | _FORMATED_FLAG:{                                                                                                              // │
                flag |= turtle::token::flag::Data::DATA_TYPE_FORMATED_UNICODE_STRING;                                                                          // │
                }break;                                                                                                                                        // │
            case _FORMATED_FLAG:{                                                                                                                              // │
                flag |= turtle::token::flag::Data::DATA_TYPE_FORMATED_STRING;                                                                                  // │
                }break;                                                                                                                                        // │
            case _RAW_FLAG:{                                                                                                                                   // │
                flag |= turtle::token::flag::Data::DATA_TYPE_RAW_STRING;                                                                                       // │
                }break;                                                                                                                                        // │
            case _UNICODE_FLAG:{                                                                                                                               // │
                flag |= turtle::token::flag::Data::DATA_TYPE_UNICODE_STRING;                                                                                   // │
                }break;                                                                                                                                        // │
            }                                                                                                                                                  // │
                                                                                                                                                               // │
        }                                                                                                                                                      // │
        [[fallthrough]];                                                                                                                                       // │
        case '"':                                                                                                                                              // │
        case '\'':{                                                                                                                                            // │
            unsigned int len = Lstr.size();                                                                                                                    // │
            if(len == 1){                                                                                                                                      // │
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

#ifdef DEBUG

        std::string res = Lstr;
        if(res[0] == '\n' || res[0] == '\r'){
            for(auto& ex : character_substitutions){
                res = std::regex_replace(res, std::regex(ex[0]), ex[1]);
            }
        }
        std::ostringstream ss;
        ss << std::bitset< sizeof(turtle_flag)*8 >(tmpNode.NodeFlags);
        const std::string& flag_str = getFlagName(tmpNode.NodeFlags);
        std::string ws; //                        The token with the largest name is 32 characters long
        for(int i=0, size = flag_str.size(); i < (33 - size); ++i){ws+=' ';}

        ss << " | Predicted token -> " << flag_str << ws
           << " | token -> [";

        //if string, format it so stays on the right margin instead of wraping in the console preview
        if(res[0] == '"' || res[0] == '\''){
            static std::regex regex(R"(\n)");
            std::vector<std::string> lines(
                std::sregex_token_iterator(res.begin(), res.end(), regex, -1),
                {}
            );
            for(uint_fast8_t i = 0; i < (uint_fast8_t)lines.size(); ++i){
                if(i > 0){ss << '\n';}
                ss << lines[i];
            }
        } else {
            ss << res;
        }
        std::cout << ss.str() << "]\n";

#endif // ifdef DEBUG

        tmpNode.line = std::move(Lexeme.lnum);
        tmpNode.linepos = std::move(Lexeme.lpos);
        Document.Nodes.push_back(tmpNode);
    }
}

} // namespace turtle

#endif // MAIN_H
