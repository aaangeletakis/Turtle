#ifndef _TURTLE_TOKEN_H
#define _TURTLE_TOKEN_H

/*
 * This file contains the magic numbers and data structure 
 * definitions nessessary to parse Python. Cython does exist, 
 * but it does not generate readable code that is a literal 
 * translation of a file. With the introduction of c++2a, 
 * almost all constructs used in python can be used in C++,
 * it's just the translation needs to be automated. 
 * This is more of my try at reinventing the wheel.
 * 
 * My motivations to do this to translate Python3 code 
 * to C++20 in order to create C++ bash scripts that run fast.
 * Also I hate python.
 * 
 * Throughout this file I make long comments -- these are not 
 * ment for you, but me, as I have a terrible memory.
 * 
 * Effort is made to use the smallest data type for the flag;
 * Currently the flag is represented by a 16 bit unsigned intager, 
 * but technically only needs 10 bits to operate.
 * 
 * I thought about using char (8 bits) for the flag but I 
 * decided to give myself a little bit of leeway.
 * 
 * Different Types of tokens are separated into classes;
 * Separating tokens into classes makes it possible to compresses
 * the information and give each types of token its unique "fingerprint"
 * or id.
 * 
 * 
 * At the parse tree stage, each deliminar          is a terminal,
 *                          each identifier or data is a non-terminal
 * 
 */

#include <string>
#include <stdint.h>
#include <vector>
#include <algorithm>

//In case I want to change it to something bigger/smaller in the future
typedef int16_t TURTLES_FLAG_DATA_TYPE;
#define turtle_flag(N) (TURTLES_FLAG_DATA_TYPE(1) << (N))

namespace turtle
{

struct TokenDataStructure
{
    //const unsigned int LineNumber;

    //Number of whitespace
    //const unsigned int Scope;

    std::string TokenString;

    TURTLES_FLAG_DATA_TYPE TokenFlags = 0;
};

struct TokenPair
{
    std::string str;
    TURTLES_FLAG_DATA_TYPE flag;
};

struct DocumentData
{
    struct DocNodePair
    {
        std::string_view str;
        TURTLES_FLAG_DATA_TYPE *flagPtr;
    };

    std::vector<std::string> TokenStrings;
    std::vector<TURTLES_FLAG_DATA_TYPE> TokenFlags;
    std::vector<DocNodePair> Tokens;

    void reserve(std::size_t n)
    {
        Tokens.reserve(n);
        TokenStrings.reserve(n);
        TokenFlags.reserve(n);
    }

    void resize(std::size_t n)
    {
        Tokens.resize(n);
        TokenStrings.resize(n);
        TokenFlags.resize(n);
    }

    void shrink_to_fit()
    {
        Tokens.shrink_to_fit();
        TokenStrings.shrink_to_fit();
        TokenFlags.shrink_to_fit();
    }

    void push_back(const TokenPair &x)
    {

        TokenStrings.push_back(x.str);
        TokenFlags.push_back(x.flag);
        Tokens.push_back({TokenStrings.back(), &TokenFlags.back()});
    }

    void push_back(TokenPair &&x)
    {
        TokenStrings.push_back(x.str);
        TokenFlags.push_back(x.flag);
        Tokens.push_back({TokenStrings.back(), &TokenFlags.back()});
    }

    void push_back(const std::string &str, const TURTLES_FLAG_DATA_TYPE &flag)
    {
        TokenStrings.push_back(str);
        TokenFlags.push_back(flag);
        Tokens.push_back({TokenStrings.back(), &TokenFlags.back()});
    }

    void push_back(std::string &&str, TURTLES_FLAG_DATA_TYPE &&flag)
    {
        TokenStrings.push_back(str);
        TokenFlags.push_back(flag);
        Tokens.push_back({TokenStrings.back(), &TokenFlags.back()});
    }
};


struct Node
{
    static struct DocumentData Data;
    static std::vector<struct Node> Nodes;
    static std::vector<TURTLES_FLAG_DATA_TYPE> dataMap;

    void *ptr[2];

    //Node as if it was and array
    void *operator[](std::size_t n)
    {
        return ptr[n];
    }

    auto left(){
        return ptr[0];
    }

    auto right(){
        return ptr[1];
    }

    constexpr auto size()
    {
        return 2;
    }
};
//                 Token String, Token Type, TokenInfo
//typedef std::tuple<std::string, char, std::bitset<32>> TokenData;
//typedef std::map<std::string, std::bitset<32>> TokenMap;

//Pseudocode for constexpr map
//typedef constexpr const TokenPair constMap;
struct TokenMapPair
{
    const char *TokenString;
    const TURTLES_FLAG_DATA_TYPE TokenFlags;
};

//This is a failure
//Update with a better system in the next failed attempt
struct NodePair
{
    //Is it a parent or a child?
    bool Type;

    //Pointer to child->parent vector
    //or
    //Pointer to child TokenDataStructure
    void *NodePtr;
};

namespace token
{

enum tokenType
{
    DELIMITERS, // such as '(' or ')' or '.' or '[' or  ']' ','
    ARITHMETIC,
    KEYWORD,    // any builtin type
    DATA,       // such as a number or string
    IDENTIFIER, // any label
    NUMBER_OF_BUILTIN_TYPES,
};

enum DataTypeTokens
{
    DATA_TYPE_STRING,
    DATA_TYPE_RAW, // Used in combination with the string flag
    DATA_TYPE_FORMATTED,
    DATA_TYPE_FORMAT_TYPE, // "%s" printf style or "{}".format style?
    DATA_TYPE_NUMBER,
    DATA_TYPE_,
    DATA_TYPE_COMMENT,
};

enum OperatorTypeTokens
{
    DELIMITER_ASSIGN, // '=' symbol
    DELIMITER_BRACE,
    DELIMITER_AT_SIGN, // '@' Python decorator, akin to passing a function pointer
    DELIMITER_COLON,   // ':' symbol
    DELIMITER_SEMICOLON,
    DELIMITER_COMMA,
    DELIMITER_PERIOD, // access token '.'
    DELIMITER_ACCESS,

    DELIMITER_LEFT_OR_RIGHT_BRACE = DELIMITER_BRACE + 1, // 0 = Left, 1 = Right
    DELIMITER_CURVED_BRACE,                              // '(' or ')'
    DELIMITER_SQUARE_BRACE,                              // '[' or ']'
    DELIMITER_CURLY_BRACE,                               // '{' or '}'
};

//like identifiers these will be represented as integers
enum KeywordTokens
{
    KEYWORD_FALSE,
    KEYWORD_CLASS,
    KEYWORD_FINALLY,
    KEYWORD_IS,
    KEYWORD_RETURN,
    KEYWORD_NONE,
    KEYWORD_CONTINUE,
    KEYWORD_FOR,
    KEYWORD_LAMBDA,
    KEYWORD_TRY,
    KEYWORD_TRUE,
    KEYWORD_DEF,
    KEYWORD_FROM,
    KEYWORD_NONLOCAL,
    KEYWORD_WHILE,
    KEYWORD_AND,
    KEYWORD_DEL,
    KEYWORD_GLOBAL,
    KEYWORD_NOT,
    KEYWORD_WITH,
    KEYWORD_AS,
    KEYWORD_ELIF,
    KEYWORD_IF,
    KEYWORD_OR,
    KEYWORD_YIELD,
    KEYWORD_ASSERT,
    KEYWORD_ELSE,
    KEYWORD_IMPORT,
    KEYWORD_PASS,
    KEYWORD_BREAK,
    KEYWORD_EXCEPT,
    KEYWORD_IN,
    KEYWORD_RAISE,
    NUMBER_OF_KEYWORDS
};

enum FlagsOfArithmeticTokens
{
    ARITHMETIC_OPERATION, // 1 = ARITHMETIC, 0 = LOGICAL (greater than, less than, equal to, not)

    ARITHMETIC_ADD,
    ARITHMETIC_SUB,
    ARITHMETIC_MULL,
    ARITHMETIC_DIV,
    ARITHMETIC_MOD,
    ARITHMETIC_FLOOR,
    ARITHMETIC_EXPONENTIAL,
    ARITHMETIC_BIT_AND,
    ARITHMETIC_BIT_OR,
    ARITHMETIC_BIT_XOR,
    ARITHMETIC_BIT_NOT,
    ARITHMETIC_BIT_LEFT_SHIFT,
    ARITHMETIC_BIT_RIGHT_SHIFT,

    ARITHMETIC_EQUAL_TO = 1,
    ARITHMETIC_GREATER_THAN,
    ARITHMETIC_LESS_THAN,
    ARITHMETIC_NOT,

    //ARITHMETIC_NOT_EQUAL_TO,

    //ARITHMETIC_GREATER_THAN_EQUAL_TO,

    //ARITHMETIC_LESS_THAN_EQUAL_TO,
};

namespace flag
{

/*
 * Below are the precomputed flags
 * 
 * This program strives for low memeory use at the cost of speed
 */

/* TokenFlags
 * 
 *    MSB    LSB
 *    |      |
 * #0 00000...[Type of Delim]   - > DELIMITERS
 * #1 00100...[Type of Arith]   - > ARITHMETIC
 * #2 01000...[Type of Keyword] - > KEYWORD
 * #3 01100...[Type of Data]    - > DATA
 * #4 10000...[Numeric id]      - > IDENTIFIER
 * 
 * The MSB in the token flags segment will be the IDENTIFIER flag.
 * If the token is not an IDENTIFIER, The token
 * types 1-4 will have a numeric id that can be extracted via
 *      (TokenFlag >> ( ( sizeof(TURTLES_FLAG_DATA_TYPE) * 8 ) - 3 ) )
 * 
 * This will make it possible to do:
 * 
 * if(TokenFlag not IDENTIFIER (i.e. the MSB is not set) ){
 *      switch(TokenFlag >> (32 bits - 3) ){
 *          case DELIMITERS:
 *          case ARITHMETIC:
 *          case KEYWORD:
 *          case DATA:
 *      }
 * }
 * 
 * or even
 * 
 * int tmp=0;
 * tmpTokenType += (TokenFlag >> (32 bits - 3) );
 * if( tmpTokenType != IDENTIFIER ){
 *      switch(tmpTokenType){
 *          ...
 *      }
 * }
 * 
 * 
 * 
 * This approach is better compared to the below as it requires 
 * fewer operations to process and saves an extra bit to be used by types 1-4
 * 
 *    MSB    LSB
 *    |      |
 * #0 10000...[Numeric id]      - > IDENTIFIER
 * #1 01000...[Type of Delim]   - > DELIMITERS
 * #2 00100...[Type of Arith]   - > ARITHMETIC
 * #3 00010...[Type of Keyword] - > KEYWORD
 * #4 00001...[Type of Data]    - > DATA
 * 
 * as this second approch would call for a FOR loop such as
 * for(int i=32 bits; i >=0; ++i){
 *      if( (TokenFlag >> i) is set){
 *          switch(32 - i){
 *              case DELIMITERS:
 *              case ARITHMETIC:
 *              case KEYWORD:
 *              case DATA:
 *          }
 *      }
 * }
 */

#define tokenTypeFlagMacro(T) ((T) << ((sizeof(TURTLES_FLAG_DATA_TYPE) * 8) - (3 /* Number of Bits needed */)))

enum tokenTypeFlags
{
    DELIMITERS = tokenTypeFlagMacro(token::DELIMITERS),
    ARITHMETIC = tokenTypeFlagMacro(token::ARITHMETIC),
    KEYWORD = tokenTypeFlagMacro(token::KEYWORD),
    DATA = tokenTypeFlagMacro(token::DATA),
    IDENTIFIER = tokenTypeFlagMacro(token::IDENTIFIER),
};

/*
    Any identifier Tokens are represented by a numeric id,
    The token flags MSB will be 1 like a signed integer to represent that it is an identifier
    To get the tokens numeric id perform
        ( flag::IDENTIFIER ^ Token.TokenFlags )
*/

enum DataTypeTokensFlags
{
    DATA_TYPE_STRING = turtle_flag(token::DATA_TYPE_STRING) | flag::DATA,
    DATA_TYPE_RAW = turtle_flag(token::DATA_TYPE_RAW) | flag::DATA,
    DATA_TYPE_COMMENT = turtle_flag(token::DATA_TYPE_COMMENT) | flag::DATA,
    DATA_TYPE_NUMBER = turtle_flag(token::DATA_TYPE_NUMBER) | flag::DATA,
    DATA_TYPE_RAW_STRING = (flag::DATA_TYPE_STRING | flag::DATA_TYPE_RAW)
};
/*
 * The DELIMITER_ASSIGN and DELIMITER_BRACE group is marked by their LSB being set
 * 
 *             ┌──> Other Deliminar tokens
 *             │  ┌──> DELIMITER_BRACE token class
 *           ┌─┴─┐│┌───> DELIMITER_ASSIGN operator class
 * 00000000 01111111
 * 
 * DELIMITER_ASSIGN operator class:
 * Any Deliminar that has the 1st LSB flag set is a ASSIGN-ment
 * 
 * DELIMITER_BRACE token class:
 * Any Deliminar that has the 2nd LSB flag set is a brace
 */

//The below may look cryptic,
//But heres whats happening:
//
//Shift the ARITHMETIC_* integer to go next
//to the DELIMITER_ASSIGN flag (DELIMITER_ASSIGN + 1 )
//
//Then give it the DELIMITER_ASSIGN flag

enum OperatorTypeTokensFlags
{
    //01 -> 00000000 00000001
    DELIMITER_ASSIGN = turtle_flag(token::DELIMITER_ASSIGN) | flag::DELIMITERS,

    //03 -> 00000000 00000011
    ARITHMETIC_ADD_ASSIGN = (token::ARITHMETIC_ADD << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //05 -> 00000000 00000101
    ARITHMETIC_SUB_ASSIGN = (token::ARITHMETIC_SUB << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //07 -> 00000000 00000111
    ARITHMETIC_MULL_ASSIGN = (token::ARITHMETIC_MULL << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //09 -> 00000000 00001001
    ARITHMETIC_DIV_ASSIGN = (token::ARITHMETIC_DIV << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //11 -> 00000000 00001011
    ARITHMETIC_MOD_ASSIGN = (token::ARITHMETIC_MOD << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //13 -> 00000000 00001101
    ARITHMETIC_FLOOR_ASSIGN = (token::ARITHMETIC_FLOOR << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //15 -> 00000000 00001111
    ARITHMETIC_EXPONENTIAL_ASSIGN = (token::ARITHMETIC_EXPONENTIAL << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //17 -> 00000000 00010001
    ARITHMETIC_BIT_AND_ASSIGN = (token::ARITHMETIC_BIT_AND << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //19 -> 00000000 00010011
    ARITHMETIC_BIT_OR_ASSIGN = (token::ARITHMETIC_BIT_OR << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //23 -> 00000000 00010111
    ARITHMETIC_BIT_XOR_ASSIGN = (token::ARITHMETIC_BIT_XOR << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //25 -> 00000000 00011001
    ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN = (token::ARITHMETIC_BIT_LEFT_SHIFT << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //27 -> 00000000 000011011
    ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN = (token::ARITHMETIC_BIT_RIGHT_SHIFT << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

    //02 -> 00000000 000000010
    DELIMITER_BRACE = turtle_flag(token::DELIMITER_BRACE) | flag::DELIMITERS,

    DELIMITER_LEFT_BRACE = turtle_flag(token::DELIMITER_LEFT_OR_RIGHT_BRACE) | flag::DELIMITERS, // 1 = Left, 0 = Right

    DELIMITER_CURVED_BRACE = turtle_flag(token::DELIMITER_CURVED_BRACE) | flag::DELIMITERS,
    DELIMITER_SQUARE_BRACE = turtle_flag(token::DELIMITER_SQUARE_BRACE) | flag::DELIMITERS,
    DELIMITER_CURLY_BRACE = turtle_flag(token::DELIMITER_CURLY_BRACE) | flag::DELIMITERS,

    DELIMITER_CURVED_RIGHT_BRACE = (DELIMITER_CURVED_BRACE | DELIMITER_BRACE),
    DELIMITER_CURVED_LEFT_BRACE = (DELIMITER_CURVED_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
    DELIMITER_SQUARE_RIGHT_BRACE = (DELIMITER_SQUARE_BRACE | DELIMITER_BRACE),
    DELIMITER_SQUARE_LEFT_BRACE = (DELIMITER_SQUARE_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
    DELIMITER_CURLY_RIGHT_BRACE = (DELIMITER_CURLY_BRACE | DELIMITER_BRACE),
    DELIMITER_CURLY_LEFT_BRACE = (DELIMITER_CURLY_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),

    DELIMITER_AT_SIGN = turtle_flag(token::DELIMITER_AT_SIGN) | flag::DELIMITERS,
    DELIMITER_COLON = turtle_flag(token::DELIMITER_COLON) | flag::DELIMITERS,
    DELIMITER_SEMICOLON = turtle_flag(token::DELIMITER_SEMICOLON) | flag::DELIMITERS,
    DELIMITER_COMMA = turtle_flag(token::DELIMITER_COMMA) | flag::DELIMITERS,
    DELIMITER_PERIOD = turtle_flag(token::DELIMITER_PERIOD) | flag::DELIMITERS,
    DELIMITER_ACCESS = turtle_flag(token::DELIMITER_ACCESS) | flag::DELIMITERS,
};

enum KeywordTokenFlags
{
    KEYWORD_FALSE = token::KEYWORD_FALSE | flag::KEYWORD,
    KEYWORD_CLASS = token::KEYWORD_CLASS | flag::KEYWORD,
    KEYWORD_FINALLY = token::KEYWORD_FINALLY | flag::KEYWORD,
    KEYWORD_IS = token::KEYWORD_IS | flag::KEYWORD,
    KEYWORD_RETURN = token::KEYWORD_RETURN | flag::KEYWORD,
    KEYWORD_NONE = token::KEYWORD_NONE | flag::KEYWORD,
    KEYWORD_CONTINUE = token::KEYWORD_CONTINUE | flag::KEYWORD,
    KEYWORD_FOR = token::KEYWORD_FOR | flag::KEYWORD,
    KEYWORD_LAMBDA = token::KEYWORD_LAMBDA | flag::KEYWORD,
    KEYWORD_TRY = token::KEYWORD_TRY | flag::KEYWORD,
    KEYWORD_TRUE = token::KEYWORD_TRUE | flag::KEYWORD,
    KEYWORD_DEF = token::KEYWORD_DEF | flag::KEYWORD,
    KEYWORD_FROM = token::KEYWORD_FROM | flag::KEYWORD,
    KEYWORD_NONLOCAL = token::KEYWORD_NONLOCAL | flag::KEYWORD,
    KEYWORD_WHILE = token::KEYWORD_WHILE | flag::KEYWORD,
    KEYWORD_AND = token::KEYWORD_AND | flag::KEYWORD,
    KEYWORD_DEL = token::KEYWORD_DEL | flag::KEYWORD,
    KEYWORD_GLOBAL = token::KEYWORD_GLOBAL | flag::KEYWORD,
    KEYWORD_NOT = token::KEYWORD_NOT | flag::KEYWORD,
    KEYWORD_WITH = token::KEYWORD_WITH | flag::KEYWORD,
    KEYWORD_AS = token::KEYWORD_AS | flag::KEYWORD,
    KEYWORD_ELIF = token::KEYWORD_ELIF | flag::KEYWORD,
    KEYWORD_IF = token::KEYWORD_IF | flag::KEYWORD,
    KEYWORD_OR = token::KEYWORD_OR | flag::KEYWORD,
    KEYWORD_YIELD = token::KEYWORD_YIELD | flag::KEYWORD,
    KEYWORD_ASSERT = token::KEYWORD_ASSERT | flag::KEYWORD,
    KEYWORD_ELSE = token::KEYWORD_ELSE | flag::KEYWORD,
    KEYWORD_IMPORT = token::KEYWORD_IMPORT | flag::KEYWORD,
    KEYWORD_PASS = token::KEYWORD_PASS | flag::KEYWORD,
    KEYWORD_BREAK = token::KEYWORD_BREAK | flag::KEYWORD,
    KEYWORD_EXCEPT = token::KEYWORD_EXCEPT | flag::KEYWORD,
    KEYWORD_IN = token::KEYWORD_IN | flag::KEYWORD,
    KEYWORD_RAISE = token::KEYWORD_RAISE | flag::KEYWORD,
};

/*
 * The '=' token is classified as a deliminar;
 * It is not an ARITHMETIC token
 */

/*
 * The type of ARITHMETIC_OPERATION is set by the LSB
 * When the LSB is 1, it is of the ARITHMETIC_OPERATION class
 * When the LSB is 0, it is of the LOGICAL_OPERATION class
 * 
 *   ┌─ ARITHMETIC flag
 *   │            ┌───> ARITHMETIC_OPERATION class
 * 0010000000000001
 * 
 * 
 *   ┌─ ARITHMETIC flag
 *   │            ┌───> LOGICAL_OPERATION class
 * 0010000000000000
 */

enum ArithmeticTokenFlags
{
    //00100000 00000001
    ARITHMETIC_OPERATION = turtle_flag(token::ARITHMETIC_OPERATION) | flag::ARITHMETIC,

    ARITHMETIC_ADD = turtle_flag(token::ARITHMETIC_ADD) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_SUB = turtle_flag(token::ARITHMETIC_SUB) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_MULL = turtle_flag(token::ARITHMETIC_MULL) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_DIV = turtle_flag(token::ARITHMETIC_DIV) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_MOD = turtle_flag(token::ARITHMETIC_MOD) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_FLOOR = turtle_flag(token::ARITHMETIC_FLOOR) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_EXPONENTIAL = turtle_flag(token::ARITHMETIC_EXPONENTIAL) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_AND = turtle_flag(token::ARITHMETIC_BIT_AND) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_OR = turtle_flag(token::ARITHMETIC_BIT_OR) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_XOR = turtle_flag(token::ARITHMETIC_BIT_XOR) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_NOT = turtle_flag(token::ARITHMETIC_BIT_NOT) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_LEFT_SHIFT = turtle_flag(token::ARITHMETIC_BIT_LEFT_SHIFT) | flag::ARITHMETIC_OPERATION,
    ARITHMETIC_BIT_RIGHT_SHIFT = turtle_flag(token::ARITHMETIC_BIT_RIGHT_SHIFT) | flag::ARITHMETIC_OPERATION,

    //0010000000000010
    ARITHMETIC_EQUAL_TO = turtle_flag(token::ARITHMETIC_EQUAL_TO) | flag::ARITHMETIC,

    //0010000000000100
    ARITHMETIC_GREATER_THAN = turtle_flag(token::ARITHMETIC_GREATER_THAN) | flag::ARITHMETIC,

    //0010000000001000
    ARITHMETIC_LESS_THAN = turtle_flag(token::ARITHMETIC_LESS_THAN) | flag::ARITHMETIC,

    //0010000000010000
    ARITHMETIC_NOT = turtle_flag(token::ARITHMETIC_NOT) | flag::ARITHMETIC,

    //0000000000000110
    ARITHMETIC_GREATER_THAN_EQUAL_TO = (flag::ARITHMETIC_EQUAL_TO | flag::ARITHMETIC_GREATER_THAN),

    //0000000000001010
    ARITHMETIC_LESS_THAN_EQUAL_TO = (flag::ARITHMETIC_EQUAL_TO | flag::ARITHMETIC_LESS_THAN),

    //0010000000010010
    ARITHMETIC_NOT_EQUAL = turtle_flag(token::ARITHMETIC_NOT) | flag::ARITHMETIC_EQUAL_TO,
};
} // namespace flag

} // namespace token

/*
 * Unfortunately std::map can not be constexpr-ed
 * Although memory is cheap, the less used the better
 * And it is my opinion to allocations on the stack make
 * for a safer garentee that the program will run effectively,
 * efficently, and fast
*/

//struct TokenPair {const char *TokenString, const unsigned long int};
constexpr const TokenMapPair turtleBuiltinTokenMap[] = {
    {"==", token::flag::ARITHMETIC_EQUAL_TO},
    {"!=", token::flag::ARITHMETIC_NOT_EQUAL},
    {"+", token::flag::ARITHMETIC_ADD},
    {"-", token::flag::ARITHMETIC_SUB},
    {"*", token::flag::ARITHMETIC_MULL},
    {"/", token::flag::ARITHMETIC_DIV},
    {"%", token::flag::ARITHMETIC_MOD},
    {">", token::flag::ARITHMETIC_GREATER_THAN},
    {"<", token::flag::ARITHMETIC_LESS_THAN},
    {"//", token::flag::ARITHMETIC_FLOOR},
    {"**", token::flag::ARITHMETIC_EXPONENTIAL},
    {"&", token::flag::ARITHMETIC_BIT_AND},
    {"|", token::flag::ARITHMETIC_BIT_OR},
    {"^", token::flag::ARITHMETIC_BIT_XOR},
    {"~", token::flag::ARITHMETIC_BIT_NOT},
    {"!", token::flag::ARITHMETIC_NOT},
    {"<<", token::flag::ARITHMETIC_BIT_LEFT_SHIFT},
    {">>", token::flag::ARITHMETIC_BIT_LEFT_SHIFT},
    {"+=", token::flag::ARITHMETIC_ADD_ASSIGN},
    {"-=", token::flag::ARITHMETIC_SUB_ASSIGN},
    {"*=", token::flag::ARITHMETIC_MULL_ASSIGN},
    {"/=", token::flag::ARITHMETIC_DIV_ASSIGN},
    {"%=", token::flag::ARITHMETIC_MOD_ASSIGN},
    {">=", token::flag::ARITHMETIC_GREATER_THAN_EQUAL_TO},
    {"<=", token::flag::ARITHMETIC_LESS_THAN_EQUAL_TO},
    {"//=", token::flag::ARITHMETIC_FLOOR_ASSIGN},
    {"**=", token::flag::ARITHMETIC_EXPONENTIAL_ASSIGN},
    {"&=", token::flag::ARITHMETIC_BIT_AND_ASSIGN},
    {"|=", token::flag::ARITHMETIC_BIT_OR_ASSIGN},
    {"^=", token::flag::ARITHMETIC_BIT_XOR_ASSIGN},
    //{"~=", token::ARITHMETIC_BIT_NOT_ASSIGN}, does not exist
    {"<<=", token::flag::ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN},
    {">>=", token::flag::ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN},

    {"False", token::flag::KEYWORD_FALSE},
    {"class", token::flag::KEYWORD_CLASS},
    {"finally", token::flag::KEYWORD_FINALLY},
    {"is", token::flag::KEYWORD_IS},
    {"return", token::flag::KEYWORD_RETURN},
    {"None", token::flag::KEYWORD_NONE},
    {"continue", token::flag::KEYWORD_CONTINUE},
    {"for", token::flag::KEYWORD_FOR},
    {"lambda", token::flag::KEYWORD_LAMBDA},
    {"try", token::flag::KEYWORD_TRY},
    {"True", token::flag::KEYWORD_TRUE},
    {"def", token::flag::KEYWORD_DEF},
    {"from", token::flag::KEYWORD_FROM},
    {"nonlocal", token::flag::KEYWORD_NONLOCAL},
    {"while", token::flag::KEYWORD_WHILE},
    {"and", token::flag::KEYWORD_AND},
    {"del", token::flag::KEYWORD_DEL},
    {"global", token::flag::KEYWORD_GLOBAL},
    {"not", token::flag::KEYWORD_NOT},
    {"with", token::flag::KEYWORD_WITH},
    {"as", token::flag::KEYWORD_AS},
    {"elif", token::flag::KEYWORD_ELIF},
    {"if", token::flag::KEYWORD_IF},
    {"or", token::flag::KEYWORD_OR},
    {"yield", token::flag::KEYWORD_YIELD},
    {"assert", token::flag::KEYWORD_ASSERT},
    {"else", token::flag::KEYWORD_ELSE},
    {"import", token::flag::KEYWORD_IMPORT},
    {"pass", token::flag::KEYWORD_PASS},
    {"break", token::flag::KEYWORD_BREAK},
    {"except", token::flag::KEYWORD_EXCEPT},
    {"in", token::flag::KEYWORD_IN},
    {"raise", token::flag::KEYWORD_RAISE}};

} // namespace turtle

#endif // _TURTLE_TOKEN_H
