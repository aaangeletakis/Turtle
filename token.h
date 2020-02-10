#ifndef TURTLE_TOKEN_H
#define TURTLE_TOKEN_H

#include <regex>
#include <map>
#include <tuple>

//#define TURTLE_TOKEN_TYPE 0
//#define TURTLE_TOKEN_TYPE_INFO 1
#define turtle_flag(F) (1 << F)

//                 Token String, Token Type, TokenInfo
typedef std::tuple<std::string, char, std::bitset<32>> TokenData;
typedef std::map<std::string, std::bitset<32>> TokenMap;

enum FlagsOfArithmeticTokens
{
    TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN,
    TURTLE_ENUM_ARITHMETIC_TOKEN_EQUAL_TO,
    TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_EQUAL,
    TURTLE_ENUM_ARITHMETIC_TOKEN_ADD,
    TURTLE_ENUM_ARITHMETIC_TOKEN_SUB,
    TURTLE_ENUM_ARITHMETIC_TOKEN_MULL,
    TURTLE_ENUM_ARITHMETIC_TOKEN_DIV,
    TURTLE_ENUM_ARITHMETIC_TOKEN_NOT,
    TURTLE_ENUM_ARITHMETIC_TOKEN_MOD, // % OPERATOR
    TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN,
    TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN,
    TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR,
    TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL,
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND,
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR,
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR,
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_NOT,
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT,
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_RIGHT_SHIFT,
    TURTLE_ENUM_NUMBER_OF_ARITHMETIC_TOKENS
};

enum FlagsOfArithmeticTokensPowerOf2
{
    TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN),
    TURTLE_ENUM_ARITHMETIC_TOKEN_EQUAL_TO_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_EQUAL_TO),
    TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_EQUAL_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_EQUAL),
    TURTLE_ENUM_ARITHMETIC_TOKEN_ADD_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_ADD),
    TURTLE_ENUM_ARITHMETIC_TOKEN_SUB_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_SUB),
    TURTLE_ENUM_ARITHMETIC_TOKEN_MULL_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_MULL),
    TURTLE_ENUM_ARITHMETIC_TOKEN_DIV_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_DIV),
    TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_NOT),
    TURTLE_ENUM_ARITHMETIC_TOKEN_MOD_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_MOD),
    TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN),
    TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN),
    TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR),
    TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_NOT_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_NOT),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_RIGHT_SHIFT_FLAG = turtle_flag(TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_RIGHT_SHIFT),
    TURTLE_ENUM_ARITHMETIC_TOKEN_ADD_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_ADD_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_SUB_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_SUB_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_MULL_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_MULL_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_DIV_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_DIV_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_MOD_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_MOD_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
    TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_RIGHT_SHIFT_ASSIGN_FLAG = (TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_RIGHT_SHIFT_FLAG | TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG),
};

//typedef std::map<std::string, std::bitset<32>> TokenMap;
TokenMap turtleArithmeticTokenMap = {
    {"=", TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG},
    {"==", TURTLE_ENUM_ARITHMETIC_TOKEN_EQUAL_TO_FLAG},
    {"is", TURTLE_ENUM_ARITHMETIC_TOKEN_EQUAL_TO_FLAG},
    {"!=", TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_EQUAL_FLAG},
    {"+", TURTLE_ENUM_ARITHMETIC_TOKEN_ADD_FLAG},
    {"-", TURTLE_ENUM_ARITHMETIC_TOKEN_SUB_FLAG},
    {"*", TURTLE_ENUM_ARITHMETIC_TOKEN_MULL_FLAG},
    {"/", TURTLE_ENUM_ARITHMETIC_TOKEN_DIV_FLAG},
    {"%", TURTLE_ENUM_ARITHMETIC_TOKEN_MOD_FLAG},
    {">", TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN_FLAG},
    {"<", TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN_FLAG},
    {"//", TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR_FLAG},
    {"**", TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL_FLAG},
    {"&", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND_FLAG},
    {"|", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR_FLAG},
    {"^", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR_FLAG},
    {"~", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_NOT_FLAG},
    {"!", TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_FLAG},
    {"not", TURTLE_ENUM_ARITHMETIC_TOKEN_NOT_FLAG},
    {"<<", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT_FLAG},
    {">>", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT_FLAG},
    {"+=", TURTLE_ENUM_ARITHMETIC_TOKEN_ADD_ASSIGN_FLAG},
    {"-=", TURTLE_ENUM_ARITHMETIC_TOKEN_SUB_ASSIGN_FLAG},
    {"*=", TURTLE_ENUM_ARITHMETIC_TOKEN_MULL_ASSIGN_FLAG},
    {"/=", TURTLE_ENUM_ARITHMETIC_TOKEN_DIV_ASSIGN_FLAG},
    {"%=", TURTLE_ENUM_ARITHMETIC_TOKEN_MOD_ASSIGN_FLAG},
    {">=", TURTLE_ENUM_ARITHMETIC_TOKEN_GREATER_THAN_ASSIGN_FLAG},
    {"<=", TURTLE_ENUM_ARITHMETIC_TOKEN_LESS_THAN_ASSIGN_FLAG},
    {"//=", TURTLE_ENUM_ARITHMETIC_TOKEN_FLOOR_ASSIGN_FLAG},
    {"**=", TURTLE_ENUM_ARITHMETIC_TOKEN_EXPONENTIAL_ASSIGN_FLAG},
    {"&=", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_AND_ASSIGN_FLAG},
    {"|=", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_OR_ASSIGN_FLAG},
    {"^=", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_XOR_ASSIGN_FLAG},
    //{"~=", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_NOT_ASSIGN_FLAG}, does not exist
    {"<<=", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_LEFT_SHIFT_ASSIGN_FLAG},
    {">>=", TURTLE_ENUM_ARITHMETIC_TOKEN_BIT_RIGHT_SHIFT_ASSIGN_FLAG},
};

//unlike the arithmetic tokens these will be represented as numbers
enum KeywordTokens
{
    TURTLE_ENUM_KEYWORD_TOKEN_FALSE,
    TURTLE_ENUM_KEYWORD_TOKEN_CLASS,
    TURTLE_ENUM_KEYWORD_TOKEN_FINALLY,
    TURTLE_ENUM_KEYWORD_TOKEN_IS,
    TURTLE_ENUM_KEYWORD_TOKEN_RETURN,
    TURTLE_ENUM_KEYWORD_TOKEN_NONE,
    TURTLE_ENUM_KEYWORD_TOKEN_CONTINUE,
    TURTLE_ENUM_KEYWORD_TOKEN_FOR,
    TURTLE_ENUM_KEYWORD_TOKEN_LAMBDA,
    TURTLE_ENUM_KEYWORD_TOKEN_TRY,
    TURTLE_ENUM_KEYWORD_TOKEN_TRUE,
    TURTLE_ENUM_KEYWORD_TOKEN_DEF,
    TURTLE_ENUM_KEYWORD_TOKEN_FROM,
    TURTLE_ENUM_KEYWORD_TOKEN_NONLOCAL,
    TURTLE_ENUM_KEYWORD_TOKEN_WHILE,
    TURTLE_ENUM_KEYWORD_TOKEN_AND,
    TURTLE_ENUM_KEYWORD_TOKEN_DEL,
    TURTLE_ENUM_KEYWORD_TOKEN_GLOBAL,
    TURTLE_ENUM_KEYWORD_TOKEN_NOT,
    TURTLE_ENUM_KEYWORD_TOKEN_WITH,
    TURTLE_ENUM_KEYWORD_TOKEN_AS,
    TURTLE_ENUM_KEYWORD_TOKEN_ELIF,
    TURTLE_ENUM_KEYWORD_TOKEN_IF,
    TURTLE_ENUM_KEYWORD_TOKEN_OR,
    TURTLE_ENUM_KEYWORD_TOKEN_YIELD,
    TURTLE_ENUM_KEYWORD_TOKEN_ASSERT,
    TURTLE_ENUM_KEYWORD_TOKEN_ELSE,
    TURTLE_ENUM_KEYWORD_TOKEN_IMPORT,
    TURTLE_ENUM_KEYWORD_TOKEN_PASS,
    TURTLE_ENUM_KEYWORD_TOKEN_BREAK,
    TURTLE_ENUM_KEYWORD_TOKEN_EXCEPT,
    TURTLE_ENUM_KEYWORD_TOKEN_IN,
    TURTLE_ENUM_KEYWORD_TOKEN_RAISE
};

//typedef std::map<std::string, std::bitset<32>> TokenMap;
TokenMap turtleKeywordTokenMap{
    {"False", TURTLE_ENUM_KEYWORD_TOKEN_FALSE},
    {"class", TURTLE_ENUM_KEYWORD_TOKEN_CLASS},
    {"finally", TURTLE_ENUM_KEYWORD_TOKEN_FINALLY},
    {"is", TURTLE_ENUM_KEYWORD_TOKEN_IS},
    {"return", TURTLE_ENUM_KEYWORD_TOKEN_RETURN},
    {"None", TURTLE_ENUM_KEYWORD_TOKEN_NONE},
    {"continue", TURTLE_ENUM_KEYWORD_TOKEN_CONTINUE},
    {"for", TURTLE_ENUM_KEYWORD_TOKEN_FOR},
    {"lambda", TURTLE_ENUM_KEYWORD_TOKEN_LAMBDA},
    {"try", TURTLE_ENUM_KEYWORD_TOKEN_TRY},
    {"True", TURTLE_ENUM_KEYWORD_TOKEN_TRUE},
    {"def", TURTLE_ENUM_KEYWORD_TOKEN_DEF},
    {"from", TURTLE_ENUM_KEYWORD_TOKEN_FROM},
    {"nonlocal", TURTLE_ENUM_KEYWORD_TOKEN_NONLOCAL},
    {"while", TURTLE_ENUM_KEYWORD_TOKEN_WHILE},
    {"and", TURTLE_ENUM_KEYWORD_TOKEN_AND},
    {"del", TURTLE_ENUM_KEYWORD_TOKEN_DEL},
    {"global", TURTLE_ENUM_KEYWORD_TOKEN_GLOBAL},
    {"not", TURTLE_ENUM_KEYWORD_TOKEN_NOT},
    {"with", TURTLE_ENUM_KEYWORD_TOKEN_WITH},
    {"as", TURTLE_ENUM_KEYWORD_TOKEN_AS},
    {"elif", TURTLE_ENUM_KEYWORD_TOKEN_ELIF},
    {"if", TURTLE_ENUM_KEYWORD_TOKEN_IF},
    {"or", TURTLE_ENUM_KEYWORD_TOKEN_OR},
    {"yield", TURTLE_ENUM_KEYWORD_TOKEN_YIELD},
    {"assert", TURTLE_ENUM_KEYWORD_TOKEN_ASSERT},
    {"else", TURTLE_ENUM_KEYWORD_TOKEN_ELSE},
    {"import", TURTLE_ENUM_KEYWORD_TOKEN_IMPORT},
    {"pass", TURTLE_ENUM_KEYWORD_TOKEN_PASS},
    {"break", TURTLE_ENUM_KEYWORD_TOKEN_BREAK},
    {"except", TURTLE_ENUM_KEYWORD_TOKEN_EXCEPT},
    {"in", TURTLE_ENUM_KEYWORD_TOKEN_IN},
    {"raise", TURTLE_ENUM_KEYWORD_TOKEN_RAISE}};

enum DataTypeTokens
{
    TURTLE_ENUM_DATA_TYPE_TOKEN_STRING,
    TURTLE_ENUM_DATA_TYPE_TOKEN_NUMBER
};

enum tokenType
{
    TURTLE_ENUM_TOKEN_KEYWORD,
    TURTLE_ENUM_TOKEN_ARITHMETIC,
    TURTLE_ENUM_NUMBER_OF_BUILTIN_TOKEN_TYPES = 2,
    TURTLE_ENUM_TOKEN_DATA = TURTLE_ENUM_NUMBER_OF_BUILTIN_TOKEN_TYPES, // such as a number or string
    TURTLE_ENUM_TOKEN_IDENTIFIER,
    TURTLE_ENUM_NUMBER_OF_TOKEN_TYPES,
    TURTLE_ENUM_NEWLINE,
};

std::array<TokenMap, TURTLE_ENUM_NUMBER_OF_BUILTIN_TOKEN_TYPES> turtleMapOfTokenTypes = {
    turtleKeywordTokenMap,
    turtleArithmeticTokenMap,
};

/*template <class T>
auto identifyAndLabelTokens(T &tokVec)
{
    std::for_each(tokVec.begin(), tokVec.end(), [&](tokenInfo &token) {
        switch (token.tokenString[0])
        {
        case '#':
        {
            token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_COMMENT_TOKEN;
            //TURTLE_ENUM_COMMENT_TOKEN is already equal to zero no need to do anything really
        }
        break;
        case '"':
        {
            //see if lable is assigned to string
            if (!token.tokenString.compare(0, 3, R"(""")"))
            {
                token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_DOCSTRING_TOKEN;
                break;
            }
            //continue to '\''
            //You may get a warning "-Wimplicit-fallthrough" --> this is intentianal
        }
        //fuking indentation is wrong hate visual studio
        case '\'':
        {
            token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_STRING_TOKEN;
            if (!(token.tokenString.length() > 3) || !strcmp(token.tokenString.c_str(), R"('\n')")) // if not a single character i.e. 'a'
            {
                token.tokenData[TURTLE_TOKEN_TYPE_INFO] += TURTLE_ENUM_SINGLE_QUOTE_STRING;
                break;
            }
            token.tokenData[TURTLE_TOKEN_TYPE_INFO] += TURTLE_ENUM_DOUBLE_QUOTE_STRING;
        }
        break;
        case '\r':
        case '\n':
        {
            token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_BEGINNING_OF_LINE_SCOPE;
            token.tokenData[TURTLE_TOKEN_TYPE_INFO] += std::count(token.tokenString.begin(), token.tokenString.begin(), ' ') + std::count(token.tokenString.begin(), token.tokenString.begin(), '\t');
        }
        break;
        case '.': // acesses modifier
        {
            token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_ACCESS_MODIFIER_TOKEN;
        }
        break;
        case '(':
            token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_CURVED_LEFT_BRACKET_TOKEN;
            break;
        case ')':
            token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_CURVED_RIGHT_BRACKET_TOKEN;
            break;
        case '=':
            token.tokenData[TURTLE_TOKEN_TYPE] = TURTLE_ENUM_ARITHMETIC_TOKEN_ASSIGN_FLAG;
            if (token.tokenString.length() == 2)
            {
                token.tokenData[TURTLE_TOKEN_TYPE]
            }
        default:
        {
            //be gone efficiency!
            for (unsigned int i = 0; i < turtleReservedTokens.size(); ++i)
            {
                //like this is like O(N^N)
                if (turtleReservedTokens[i].count(token.tokenString.c_str()))
                {
                    token.tokenData[TURTLE_TOKEN_TYPE] += i + TURTLE_ENUM_START_OF_RESERVED_TOKEN_MAP_TYPES;
                    token.tokenData[TURTLE_TOKEN_TYPE_INFO] += turtleReservedTokens[i].at(token.tokenString.c_str());
                    break;
                }
            }
            //token.tokenData[TURTLE_TOKEN_TYPE] += TURTLE_ENUM_OTHER_UNKOWN_TOKEN;
        }
        break;
        }
    });
}*/

#endif // TURTLE_TOKEN_H