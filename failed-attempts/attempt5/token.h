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
 * This projects purpose is to translate Python3 code 
 * to C++20 in order to create C++ bash scripts that 
 * can be compiled and run faster than Python code
 */

#include <string>

//In case I want to change it to something bigger in the future
#define TURTLES_FLAG_DATA_TYPE unsigned long int

#define turtle_flag(N) (1ul << (N))

//Set MSB then shift over by N
#define turtle_token_type_flag(N) \
    ((~(~0ul >> 1)) >> (N))

namespace turtle
{

struct TokenDataStructure
{
    //Line number
    const unsigned int LineIndex;
    
    //Number of whitespace
    const unsigned int Scope;
    
    //What number token is this
    const unsigned int TokenIndex;

    std::string TokenString;

    TURTLES_FLAG_DATA_TYPE TokenFlags = 0;
};

//                 Token String, Token Type, TokenInfo
//typedef std::tuple<std::string, char, std::bitset<32>> TokenData;
//typedef std::map<std::string, std::bitset<32>> TokenMap;

//Pseudocode for constexpr map
//typedef constexpr const TokenPair constMap;
struct TokenPair
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
    IDENTIFIER,
    DELIMITERS, // such as '(' or ')' or '.' or '[' or  ']' ','
    ARITHMETIC,
    KEYWORD,
    DATA, // such as a number or string
    NUMBER_OF_BUILTIN_TYPES,
};

enum DataTypeTokens
{
    DATA_TYPE_STRING,
    DATA_TYPE_RAW, // Used in combination with the string flag
    DATA_TYPE_COMMENT,
    DATA_TYPE_NUMBER,
};

enum OperatorTypeTokens
{
    DELIMITER_AT_SIGN, //'@' is the same as 'def'
    DELIMITER_COLON,   // ':' symbol
    DELIMITER_SEMICOLON,
    DELIMITER_COMMA,
    DELIMITER_PERIOD, // acssess token '.'
    DELIMITER_ACCESS,
    DELIMITER_BRACE,
    DELIMITER_LEFT_BRACE,
    DELIMITER_RIGHT_BRACE,
    DELIMITER_CURVED_BRACE, // '(' or ')'
    DELIMITER_SQUARE_BRACE, // '[' or ']'
    DELIMITER_CURLY_BRACE   // '{' or '}'
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
    ARITHMETIC_EQUAL_TO,
    ARITHMETIC_ASSIGN,
    ARITHMETIC_NOT_EQUAL,
    ARITHMETIC_ADD,
    ARITHMETIC_SUB,
    ARITHMETIC_MULL,
    ARITHMETIC_DIV,
    ARITHMETIC_NOT,
    ARITHMETIC_MOD, // % OPERATOR
    ARITHMETIC_GREATER_THAN,
    ARITHMETIC_LESS_THAN,
    ARITHMETIC_FLOOR,
    ARITHMETIC_EXPONENTIAL,
    ARITHMETIC_BIT_AND,
    ARITHMETIC_BIT_OR,
    ARITHMETIC_BIT_XOR,
    ARITHMETIC_BIT_NOT,
    ARITHMETIC_BIT_LEFT_SHIFT,
    ARITHMETIC_BIT_RIGHT_SHIFT,
};

namespace flag
{

/*
 * Below are the precomputed flags
 */

/* TokenFlags
 * 
 * MSB    LSB
 * |      |
 * 10000...[Numeric id]      - > IDENTIFIER
 * 01000...[Type of Delim]   - > DELIMITERS
 * 00100...[Type of Arith]   - > ARITHMETIC
 * 00010...[Type of Keyword] - > KEYWORD
 * 00001...[Type of Data]    - > DATA
 */

enum tokenTypeFlags
{
    IDENTIFIER = turtle_token_type_flag(IDENTIFIER),
    DELIMITERS = turtle_token_type_flag(DELIMITERS),
    ARITHMETIC = turtle_token_type_flag(ARITHMETIC),
    KEYWORD = turtle_token_type_flag(KEYWORD),
    DATA = turtle_token_type_flag(DATA),
};

/*
    Any identifier Tokens are represented by a numeric id,
    The token flags MSB will be 1 like a signed integer to represent that it is an identifier
    To get the tokens numeric id perform
        ( turtle::token::flag::IDENTIFIER ^ Token.TokenFlags )
*/

enum DataTypeTokensFlags
{
    DATA_TYPE_STRING = turtle_flag(DATA_TYPE_STRING) | DATA,
    DATA_TYPE_RAW = turtle_flag(DATA_TYPE_RAW) | DATA,
    DATA_TYPE_COMMENT = turtle_flag(DATA_TYPE_COMMENT) | DATA,
    DATA_TYPE_NUMBER = turtle_flag(DATA_TYPE_NUMBER) | DATA,
    DATA_TYPE_RAW_STRING = (DATA_TYPE_STRING | DATA_TYPE_RAW)
};

enum OperatorTypeTokensFlags
{
    DELIMITER_AT_SIGN = turtle_flag(DELIMITER_AT_SIGN) | DELIMITERS,
    DELIMITER_COLON = turtle_flag(DELIMITER_COLON) | DELIMITERS,
    DELIMITER_SEMICOLON = turtle_flag(DELIMITER_SEMICOLON) | DELIMITERS,
    DELIMITER_COMMA = turtle_flag(DELIMITER_COMMA) | DELIMITERS,
    DELIMITER_PERIOD = turtle_flag(DELIMITER_PERIOD) | DELIMITERS,
    DELIMITER_ACCESS = turtle_flag(DELIMITER_ACCESS) | DELIMITERS,
    DELIMITER_BRACE = turtle_flag(DELIMITER_BRACE) | DELIMITERS,
    DELIMITER_LEFT_BRACE = turtle_flag(DELIMITER_LEFT_BRACE) | DELIMITERS,
    DELIMITER_RIGHT_BRACE = turtle_flag(DELIMITER_RIGHT_BRACE) | DELIMITERS,
    DELIMITER_CURVED_BRACE = turtle_flag(DELIMITER_CURVED_BRACE) | DELIMITERS,
    DELIMITER_SQUARE_BRACE = turtle_flag(DELIMITER_SQUARE_BRACE) | DELIMITERS,
    DELIMITER_CURLY_BRACE = turtle_flag(DELIMITER_CURLY_BRACE) | DELIMITERS,

    DELIMITER_CURVED_RIGHT_BRACE = (DELIMITER_CURVED_BRACE | DELIMITER_RIGHT_BRACE | DELIMITER_BRACE),
    DELIMITER_CURVED_LEFT_BRACE = (DELIMITER_CURVED_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
    DELIMITER_SQUARE_RIGHT_BRACE = (DELIMITER_SQUARE_BRACE | DELIMITER_RIGHT_BRACE | DELIMITER_BRACE),
    DELIMITER_SQUARE_LEFT_BRACE = (DELIMITER_SQUARE_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
    DELIMITER_CURLY_RIGHT_BRACE = (DELIMITER_CURLY_BRACE | DELIMITER_RIGHT_BRACE | DELIMITER_BRACE),
    DELIMITER_CURLY_LEFT_BRACE = (DELIMITER_CURLY_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
};

enum KeywordTokenFlags
{
    KEYWORD_FALSE = KEYWORD_FALSE | KEYWORD,
    KEYWORD_CLASS = KEYWORD_CLASS | KEYWORD,
    KEYWORD_FINALLY = KEYWORD_FINALLY | KEYWORD,
    KEYWORD_IS = KEYWORD_IS | KEYWORD,
    KEYWORD_RETURN = KEYWORD_RETURN | KEYWORD,
    KEYWORD_NONE = KEYWORD_NONE | KEYWORD,
    KEYWORD_CONTINUE = KEYWORD_CONTINUE | KEYWORD,
    KEYWORD_FOR = KEYWORD_FOR | KEYWORD,
    KEYWORD_LAMBDA = KEYWORD_LAMBDA | KEYWORD,
    KEYWORD_TRY = KEYWORD_TRY | KEYWORD,
    KEYWORD_TRUE = KEYWORD_TRUE | KEYWORD,
    KEYWORD_DEF = KEYWORD_DEF | KEYWORD,
    KEYWORD_FROM = KEYWORD_FROM | KEYWORD,
    KEYWORD_NONLOCAL = KEYWORD_NONLOCAL | KEYWORD,
    KEYWORD_WHILE = KEYWORD_WHILE | KEYWORD,
    KEYWORD_AND = KEYWORD_AND | KEYWORD,
    KEYWORD_DEL = KEYWORD_DEL | KEYWORD,
    KEYWORD_GLOBAL = KEYWORD_GLOBAL | KEYWORD,
    KEYWORD_NOT = KEYWORD_NOT | KEYWORD,
    KEYWORD_WITH = KEYWORD_WITH | KEYWORD,
    KEYWORD_AS = KEYWORD_AS | KEYWORD,
    KEYWORD_ELIF = KEYWORD_ELIF | KEYWORD,
    KEYWORD_IF = KEYWORD_IF | KEYWORD,
    KEYWORD_OR = KEYWORD_OR | KEYWORD,
    KEYWORD_YIELD = KEYWORD_YIELD | KEYWORD,
    KEYWORD_ASSERT = KEYWORD_ASSERT | KEYWORD,
    KEYWORD_ELSE = KEYWORD_ELSE | KEYWORD,
    KEYWORD_IMPORT = KEYWORD_IMPORT | KEYWORD,
    KEYWORD_PASS = KEYWORD_PASS | KEYWORD,
    KEYWORD_BREAK = KEYWORD_BREAK | KEYWORD,
    KEYWORD_EXCEPT = KEYWORD_EXCEPT | KEYWORD,
    KEYWORD_IN = KEYWORD_IN | KEYWORD,
    KEYWORD_RAISE = KEYWORD_RAISE | KEYWORD,
};

enum FlagsOfArithmeticTokensPowerOf2
{
    ARITHMETIC_ASSIGN = turtle_flag(ARITHMETIC_ASSIGN) | ARITHMETIC,
    ARITHMETIC_EQUAL_TO = turtle_flag(ARITHMETIC_EQUAL_TO) | ARITHMETIC,
    ARITHMETIC_NOT_EQUAL = turtle_flag(ARITHMETIC_NOT_EQUAL) | ARITHMETIC,
    ARITHMETIC_ADD = turtle_flag(ARITHMETIC_ADD) | ARITHMETIC,
    ARITHMETIC_SUB = turtle_flag(ARITHMETIC_SUB) | ARITHMETIC,
    ARITHMETIC_MULL = turtle_flag(ARITHMETIC_MULL) | ARITHMETIC,
    ARITHMETIC_DIV = turtle_flag(ARITHMETIC_DIV) | ARITHMETIC,
    ARITHMETIC_NOT = turtle_flag(ARITHMETIC_NOT) | ARITHMETIC,
    ARITHMETIC_MOD = turtle_flag(ARITHMETIC_MOD) | ARITHMETIC,
    ARITHMETIC_GREATER_THAN = turtle_flag(ARITHMETIC_GREATER_THAN) | ARITHMETIC,
    ARITHMETIC_LESS_THAN = turtle_flag(ARITHMETIC_LESS_THAN) | ARITHMETIC,
    ARITHMETIC_FLOOR = turtle_flag(ARITHMETIC_FLOOR) | ARITHMETIC,
    ARITHMETIC_EXPONENTIAL = turtle_flag(ARITHMETIC_EXPONENTIAL) | ARITHMETIC,
    ARITHMETIC_BIT_AND = turtle_flag(ARITHMETIC_BIT_AND) | ARITHMETIC,
    ARITHMETIC_BIT_OR = turtle_flag(ARITHMETIC_BIT_OR) | ARITHMETIC,
    ARITHMETIC_BIT_XOR = turtle_flag(ARITHMETIC_BIT_XOR) | ARITHMETIC,
    ARITHMETIC_BIT_NOT = turtle_flag(ARITHMETIC_BIT_NOT) | ARITHMETIC,
    ARITHMETIC_BIT_LEFT_SHIFT = turtle_flag(ARITHMETIC_BIT_LEFT_SHIFT) | ARITHMETIC,
    ARITHMETIC_BIT_RIGHT_SHIFT = turtle_flag(ARITHMETIC_BIT_RIGHT_SHIFT) | ARITHMETIC,

    ARITHMETIC_ADD_ASSIGN = (ARITHMETIC_ADD | ARITHMETIC_ASSIGN),
    ARITHMETIC_SUB_ASSIGN = (ARITHMETIC_SUB | ARITHMETIC_ASSIGN),
    ARITHMETIC_MULL_ASSIGN = (ARITHMETIC_MULL | ARITHMETIC_ASSIGN),
    ARITHMETIC_DIV_ASSIGN = (ARITHMETIC_DIV | ARITHMETIC_ASSIGN),
    ARITHMETIC_MOD_ASSIGN = (ARITHMETIC_MOD | ARITHMETIC_ASSIGN),
    ARITHMETIC_GREATER_THAN_ASSIGN = (ARITHMETIC_GREATER_THAN | ARITHMETIC_ASSIGN),
    ARITHMETIC_LESS_THAN_ASSIGN = (ARITHMETIC_LESS_THAN | ARITHMETIC_ASSIGN),
    ARITHMETIC_FLOOR_ASSIGN = (ARITHMETIC_FLOOR | ARITHMETIC_ASSIGN),
    ARITHMETIC_EXPONENTIAL_ASSIGN = (ARITHMETIC_EXPONENTIAL | ARITHMETIC_ASSIGN),
    ARITHMETIC_BIT_AND_ASSIGN = (ARITHMETIC_BIT_AND | ARITHMETIC_ASSIGN),
    ARITHMETIC_BIT_OR_ASSIGN = (ARITHMETIC_BIT_OR | ARITHMETIC_ASSIGN),
    ARITHMETIC_BIT_XOR_ASSIGN = (ARITHMETIC_BIT_XOR | ARITHMETIC_ASSIGN),
    ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN = (ARITHMETIC_BIT_LEFT_SHIFT | ARITHMETIC_ASSIGN),
    ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN = (ARITHMETIC_BIT_RIGHT_SHIFT | ARITHMETIC_ASSIGN),
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
constexpr const TokenPair turtleBuiltinTokenMap[] = {
    {"=", token::flag::ARITHMETIC_ASSIGN},
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
    {">=", token::flag::ARITHMETIC_GREATER_THAN_ASSIGN},
    {"<=", token::flag::ARITHMETIC_LESS_THAN_ASSIGN},
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
