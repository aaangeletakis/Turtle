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
 * 
 * Throughout this file I make long comments -- these are not 
 * ment for you, but me, as I have a terrible memory.
 * 
 * The use of enums are really a great thing as they can be 
 * used as pre-calculated magic numbers
 *
 * Currently the flag is represented by a 32 bit unsigned integer.
 * 
 *
 * Different Types of tokens are separated into classes;
 * Separating tokens into classes makes it possible to compresses
 * the information and give each types of token its unique "fingerprint"
 * or id.
 * 
 * Once a specific token has been identified as some sort of predefined or
 * identifier it's token string is deleted from program memory, as 
 * it is more efficent to just store the identifying "fingerprint" of a token
 * then a multi character string.
 *      Example:
 *          The '(' left brace token is predefined, give it the numeric id 
 *          DELIMITER_CURVED_LEFT_BRACE, delete token string and go to 
 *          the next token
 * 
 * At the parse tree stage, each deliminar          is a     terminal,
 *                          each identifier or data is a non-terminal
 * 
 */

#include <string>
#include <stdint.h>
#include <string.h>
#include "global.h"

//In case I want to change it to something bigger/smaller in the future
typedef uint32_t turtle_flag;

//      M_turtle_flag(N) (00000000 00000000 00000000 00000001 <<  N )
#define M_turtle_flag(N) (turtle_flag(1) << (N))

//Refer to the huge comment in the flag namespace on wtf this is & does
#define M_tokenTypeFlagMacro(N) (N << ((sizeof(turtle_flag) * 8) - (3 /* Number of Bits needed */ + 1)))

//#define M_getBit(N)  sizeof(TURTLES_FLAG_DATA_TYPE) * 8)

struct Node
{
    //Node
    turtle_flag NodeFlags = 0;
    Node *Children[2] = {0};
    auto getTokenType()
    {
        return NodeFlags >> ((sizeof(turtle_flag) * 8) - (3 /* Number of Bits needed */ + 1));
    }
};

namespace turtle
{

    struct TokenDataStructure
    {
        /*const unsigned int LineNumber;*/

        /*
         * Number of whitespace
         * const unsigned int Scope;
         */

        turtle_flag TokenFlags = 0;
        std::string TokenString;
    };

    //std::vector<struct Node> SyntaxGroups
    //         ||
    //         \/
    //std::vector<struct Node> SemanticGroups

    //convert 8 byte string to 64 bit intager
    constexpr uint_fast64_t sti(const char *str)
    {
        uint_fast64_t res = 0;
        for (uint_fast8_t i = 0; str[i]; ++i)
        {
            res <<= 8;
            res |= str[i];
        }
        return res;
    }

    namespace token
    {

        enum tokenType
        {
            CONTROL,
            DELIMITERS, // such as '(' or ')' or '.' or '[' or ']' ','
            ARITHMETIC,
            KEYWORD,    // any builtin type
            DATA,       // such as a number or string
            IDENTIFIER, // any label
            NUMBER_OF_BUILTIN_TYPES,
        };

        enum ControlTypeTokens
        {
            NULL_TOKEN,
            NEWLINE,
            WHITESPACE
        };

        enum DataTypeTokens
        {
            DATA_TYPE_STRING,
            DATA_TYPE_RAW, // Used in combination with the string flag
            DATA_TYPE_FORMATTED,
            DATA_TYPE_FORMAT_TYPE, // "%s" printf style or "{}".format style?
            DATA_TYPE_NUMBER,
            DATA_TYPE_, //unknown -- make effort to determine type
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
            //DELIMITER_ACCESS,

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

            ARITHMETIC_EQUAL_TO = 1, //LOGICAL CLASS
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
 *
 *  TokenFlags
 * 
 *    MSB    LSB
 *    |      |
 * #1 000...[Type of Control] - > CONTROL
 * #2 001...[Type of Delim]   - > DELIMITERS
 * #3 010...[Type of Arith]   - > ARITHMETIC
 * #4 011...[Type of Keyword] - > KEYWORD
 * #5 100...[Type of Data]    - > DATA
 * #6 101...[Numeric id]      - > IDENTIFIER
 * 
 * The MSB in the token flags segment will be the IDENTIFIER flag.
 * If the token is not an IDENTIFIER, The token
 * types 2-6 will have a numeric id that can be extracted via
 *      (TokenFlag >> ( ( sizeof(TURTLES_FLAG_DATA_TYPE) * 8 ) - 4 ) )
 * 
 * This will make it possible to do:
 * 
 * if(TokenFlag not IDENTIFIER (i.e. the MSB is not set) ){
 *      switch(TokenFlag >> (32 bits - 4) ){
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
 * tmpTokenType += (TokenFlag >> (32 bits - 4) );
 * if( tmpTokenType != IDENTIFIER ){
 *      switch(tmpTokenType){
 *          ...
 *      }
 * }
 */

            enum tokenTypeFlags
            {
                CONTROL =    M_tokenTypeFlagMacro(token::CONTROL),
                DELIMITERS = M_tokenTypeFlagMacro(token::DELIMITERS),
                ARITHMETIC = M_tokenTypeFlagMacro(token::ARITHMETIC),
                KEYWORD =    M_tokenTypeFlagMacro(token::KEYWORD),
                DATA =       M_tokenTypeFlagMacro(token::DATA),
                IDENTIFIER = M_tokenTypeFlagMacro(token::IDENTIFIER),
            };

            /*
             *
             *   ┌──> Flag Type - Control Class Id
             *   │
             * ┌─┤                                ┌───> Is null token
             * 00000000  00000000 00000000 00000000
             *
             *
             *
             *
             *   ┌──> Flag Type - Control Class Id
             *   │                            ┌──> Amount Of whitespace (max 2,048 characters)
             * ┌─┤                    ┌───────┴──┐┌───> Is newline
             * 00011111  11111111 11111111 11111111
             *    │                  │
             *    └──────────────────┴──> Line Number (max 65,536 lines)
             */

            enum ControlTypeFlags
            {
                NULL_TOKEN = M_turtle_flag(token::NULL_TOKEN) | flag::CONTROL,
                NEWLINE = M_turtle_flag(token::NEWLINE) | flag::CONTROL,
            };

            /*
                Any identifier Tokens are represented by a numeric id,
                To get the tokens numeric id perform
                    ( flag::IDENTIFIER XOR Node.NodeFlag )


                  ┌──> Flag Type - IDENTIFIER Class Id
                  │
                ┌─┤
                10100000  00000000 00000000 00000000
                   └───────────────────────────────┴──> Numeric Id
            */

            /*
                //regular string class
                  ┌──> Flag Type - DATA Class Id
                  │
                ┌─┤
                10000000  00000000 00000000 00001111
                                                │││└──> Is string
                                                ││└───> Is raw string
                                                │└────> Is formated string
                                                └─────> Format Type

               //Unicode Strings
                  ┌──> Flag Type - DATA Class Id
                  │
                ┌─┤
                10000000  00000000 00000000 00001001
                                                │││└──> Is string
                                                ││└───> Is raw string
                                                │└────> Is formated string
                                                └─────> Is unicode string
             */
            enum DataTypeTokensFlags
            {
                DATA_TYPE_STRING =     M_turtle_flag(token::DATA_TYPE_STRING)  | flag::DATA,
                DATA_TYPE_RAW =        M_turtle_flag(token::DATA_TYPE_RAW)     | flag::DATA,
                DATA_TYPE_COMMENT =    M_turtle_flag(token::DATA_TYPE_COMMENT) | flag::DATA,
                DATA_TYPE_NUMBER =     M_turtle_flag(token::DATA_TYPE_NUMBER)  | flag::DATA,
                DATA_TYPE_RAW_STRING = (flag::DATA_TYPE_STRING | flag::DATA_TYPE_RAW)
            };

            /*
             * The DELIMITER_ASSIGN and DELIMITER_BRACE group is marked by their LSB being set
             *
             *
             *   ┌──> Flag Type - Deliminar Class Id
             *   │                            ┌──> Other Deliminar tokens
             *   │                            │  ┌──> DELIMITER_BRACE token class
             * ┌─┤                          ┌─┴─┐│┌───> DELIMITER_ASSIGN operator class
             * 00100000  00000000 00000000 01111111
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
            //Shift an arithmetic integer to go next to the DELIMITER_ASSIGN flag (DELIMITER_ASSIGN + 1 )
            //
            //Then give it the DELIMITER_ASSIGN flag

            enum OperatorTypeTokensFlags
            {
                //01 -> 00000000 00000001
                DELIMITER_ASSIGN = M_turtle_flag(token::DELIMITER_ASSIGN) | flag::DELIMITERS,
                ARITHMETIC_ADD_ASSIGN = (token::ARITHMETIC_ADD << (token::DELIMITER_ASSIGN + 1) |
                                         flag::DELIMITER_ASSIGN),
                ARITHMETIC_SUB_ASSIGN = (token::ARITHMETIC_SUB << (token::DELIMITER_ASSIGN + 1) |
                                         flag::DELIMITER_ASSIGN),
                ARITHMETIC_MULL_ASSIGN = (token::ARITHMETIC_MULL << (token::DELIMITER_ASSIGN + 1) |
                                          flag::DELIMITER_ASSIGN),
                ARITHMETIC_DIV_ASSIGN = (token::ARITHMETIC_DIV << (token::DELIMITER_ASSIGN + 1) |
                                         flag::DELIMITER_ASSIGN),
                ARITHMETIC_MOD_ASSIGN = (token::ARITHMETIC_MOD << (token::DELIMITER_ASSIGN + 1) |
                                         flag::DELIMITER_ASSIGN),
                ARITHMETIC_FLOOR_ASSIGN = (token::ARITHMETIC_FLOOR << (token::DELIMITER_ASSIGN + 1) |
                                           flag::DELIMITER_ASSIGN),
                ARITHMETIC_EXPONENTIAL_ASSIGN = (token::ARITHMETIC_EXPONENTIAL << (token::DELIMITER_ASSIGN + 1) |
                                                 flag::DELIMITER_ASSIGN),
                ARITHMETIC_BIT_AND_ASSIGN = (token::ARITHMETIC_BIT_AND << (token::DELIMITER_ASSIGN + 1) |
                                             flag::DELIMITER_ASSIGN),
                ARITHMETIC_BIT_OR_ASSIGN = (token::ARITHMETIC_BIT_OR << (token::DELIMITER_ASSIGN + 1) |
                                            flag::DELIMITER_ASSIGN),
                ARITHMETIC_BIT_XOR_ASSIGN = (token::ARITHMETIC_BIT_XOR << (token::DELIMITER_ASSIGN + 1) |
                                             flag::DELIMITER_ASSIGN),
                ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN = (token::ARITHMETIC_BIT_LEFT_SHIFT << (token::DELIMITER_ASSIGN + 1) |
                                                    flag::DELIMITER_ASSIGN),
                ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN = (token::ARITHMETIC_BIT_RIGHT_SHIFT << (token::DELIMITER_ASSIGN + 1) | flag::DELIMITER_ASSIGN),

                DELIMITER_BRACE = M_turtle_flag(token::DELIMITER_BRACE) | flag::DELIMITERS,

                DELIMITER_LEFT_BRACE =
                    M_turtle_flag(token::DELIMITER_LEFT_OR_RIGHT_BRACE) | flag::DELIMITERS, // 1 = Left, 0 = Right

                DELIMITER_CURVED_BRACE = M_turtle_flag(token::DELIMITER_CURVED_BRACE) | flag::DELIMITERS,
                DELIMITER_SQUARE_BRACE = M_turtle_flag(token::DELIMITER_SQUARE_BRACE) | flag::DELIMITERS,
                DELIMITER_CURLY_BRACE =  M_turtle_flag(token::DELIMITER_CURLY_BRACE)  | flag::DELIMITERS,

                DELIMITER_CURVED_RIGHT_BRACE = (DELIMITER_CURVED_BRACE                        | DELIMITER_BRACE),
                DELIMITER_CURVED_LEFT_BRACE =  (DELIMITER_CURVED_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
                DELIMITER_SQUARE_RIGHT_BRACE = (DELIMITER_SQUARE_BRACE                        | DELIMITER_BRACE),
                DELIMITER_SQUARE_LEFT_BRACE =  (DELIMITER_SQUARE_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
                DELIMITER_CURLY_RIGHT_BRACE =  (DELIMITER_CURLY_BRACE                         | DELIMITER_BRACE),
                DELIMITER_CURLY_LEFT_BRACE =   (DELIMITER_CURLY_BRACE  | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),

                DELIMITER_AT_SIGN =      M_turtle_flag(token::DELIMITER_AT_SIGN)     | flag::DELIMITERS,
                DELIMITER_COLON =        M_turtle_flag(token::DELIMITER_COLON)       | flag::DELIMITERS,
                DELIMITER_SEMICOLON =    M_turtle_flag(token::DELIMITER_SEMICOLON)   | flag::DELIMITERS,
                DELIMITER_COMMA =        M_turtle_flag(token::DELIMITER_COMMA)       | flag::DELIMITERS,
                DELIMITER_PERIOD =       M_turtle_flag(token::DELIMITER_PERIOD)      | flag::DELIMITERS,
                //DELIMITER_ACCESS = M_turtle_flag(token::DELIMITER_ACCESS) | flag::DELIMITERS,
            };

            enum KeywordTokenFlags
            {
                KEYWORD_FALSE =      token::KEYWORD_FALSE    | flag::KEYWORD,
                KEYWORD_CLASS =      token::KEYWORD_CLASS    | flag::KEYWORD,
                KEYWORD_FINALLY =    token::KEYWORD_FINALLY  | flag::KEYWORD,
                KEYWORD_IS =         token::KEYWORD_IS       | flag::KEYWORD,
                KEYWORD_RETURN =     token::KEYWORD_RETURN   | flag::KEYWORD,
                KEYWORD_NONE =       token::KEYWORD_NONE     | flag::KEYWORD,
                KEYWORD_CONTINUE =   token::KEYWORD_CONTINUE | flag::KEYWORD,
                KEYWORD_FOR =        token::KEYWORD_FOR      | flag::KEYWORD,
                KEYWORD_LAMBDA =     token::KEYWORD_LAMBDA   | flag::KEYWORD,
                KEYWORD_TRY =        token::KEYWORD_TRY      | flag::KEYWORD,
                KEYWORD_TRUE =       token::KEYWORD_TRUE     | flag::KEYWORD,
                KEYWORD_DEF =        token::KEYWORD_DEF      | flag::KEYWORD,
                KEYWORD_FROM =       token::KEYWORD_FROM     | flag::KEYWORD,
                KEYWORD_NONLOCAL =   token::KEYWORD_NONLOCAL | flag::KEYWORD,
                KEYWORD_WHILE =      token::KEYWORD_WHILE    | flag::KEYWORD,
                KEYWORD_AND =        token::KEYWORD_AND      | flag::KEYWORD,
                KEYWORD_DEL =        token::KEYWORD_DEL      | flag::KEYWORD,
                KEYWORD_GLOBAL =     token::KEYWORD_GLOBAL   | flag::KEYWORD,
                KEYWORD_NOT =        token::KEYWORD_NOT      | flag::KEYWORD,
                KEYWORD_WITH =       token::KEYWORD_WITH     | flag::KEYWORD,
                KEYWORD_AS =         token::KEYWORD_AS       | flag::KEYWORD,
                KEYWORD_ELIF =       token::KEYWORD_ELIF     | flag::KEYWORD,
                KEYWORD_IF =         token::KEYWORD_IF       | flag::KEYWORD,
                KEYWORD_OR =         token::KEYWORD_OR       | flag::KEYWORD,
                KEYWORD_YIELD =      token::KEYWORD_YIELD    | flag::KEYWORD,
                KEYWORD_ASSERT =     token::KEYWORD_ASSERT   | flag::KEYWORD,
                KEYWORD_ELSE =       token::KEYWORD_ELSE     | flag::KEYWORD,
                KEYWORD_IMPORT =     token::KEYWORD_IMPORT   | flag::KEYWORD,
                KEYWORD_PASS =       token::KEYWORD_PASS     | flag::KEYWORD,
                KEYWORD_BREAK =      token::KEYWORD_BREAK    | flag::KEYWORD,
                KEYWORD_EXCEPT =     token::KEYWORD_EXCEPT   | flag::KEYWORD,
                KEYWORD_IN =         token::KEYWORD_IN       | flag::KEYWORD,
                KEYWORD_RAISE =      token::KEYWORD_RAISE    | flag::KEYWORD,
            };

            /*
             * The '=' token is classified as a deliminar;
             * It is NOT an ARITHMETIC token
             */

            /*
             * The type of ARITHMETIC_OPERATION is set by the LSB
             * When the LSB is 1, it is of the ARITHMETIC_OPERATION class
             * When the LSB is 0, it is of the LOGICAL_OPERATION class
             *
             *   ┌─ Flag Type - ARITHMETIC Class Id
             * ┌─┤                               ┌───> ARITHMETIC_OPERATION class
             * 01000000 00000000 00000000 00000001
             *
             *
             *   ┌─ Flag Type - ARITHMETIC Class Id
             * ┌─┤                               ┌───> LOGICAL_OPERATION class
             * 01000000 00000000 00000000 00000000
             */

            enum ArithmeticTokenFlags
            {
                ARITHMETIC_OPERATION =       M_turtle_flag(token::ARITHMETIC_OPERATION)   | flag::ARITHMETIC,

                ARITHMETIC_ADD =             M_turtle_flag(token::ARITHMETIC_ADD)             | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_SUB =             M_turtle_flag(token::ARITHMETIC_SUB)             | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_MULL =            M_turtle_flag(token::ARITHMETIC_MULL)            | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_DIV =             M_turtle_flag(token::ARITHMETIC_DIV)             | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_MOD =             M_turtle_flag(token::ARITHMETIC_MOD)             | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_FLOOR =           M_turtle_flag(token::ARITHMETIC_FLOOR)           | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_EXPONENTIAL =     M_turtle_flag(token::ARITHMETIC_EXPONENTIAL)     | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_AND =         M_turtle_flag(token::ARITHMETIC_BIT_AND)         | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_OR =          M_turtle_flag(token::ARITHMETIC_BIT_OR)          | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_XOR =         M_turtle_flag(token::ARITHMETIC_BIT_XOR)         | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_NOT =         M_turtle_flag(token::ARITHMETIC_BIT_NOT)         | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_LEFT_SHIFT =  M_turtle_flag(token::ARITHMETIC_BIT_LEFT_SHIFT)  | flag::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_RIGHT_SHIFT = M_turtle_flag(token::ARITHMETIC_BIT_RIGHT_SHIFT) | flag::ARITHMETIC_OPERATION,

                ARITHMETIC_EQUAL_TO =        M_turtle_flag(token::ARITHMETIC_EQUAL_TO)        | flag::ARITHMETIC,

                ARITHMETIC_NOT_EQUAL =       M_turtle_flag(token::ARITHMETIC_NOT)             | flag::ARITHMETIC_EQUAL_TO,

                ARITHMETIC_GREATER_THAN =    M_turtle_flag(token::ARITHMETIC_GREATER_THAN)    | flag::ARITHMETIC,

                ARITHMETIC_LESS_THAN =       M_turtle_flag(token::ARITHMETIC_LESS_THAN)       | flag::ARITHMETIC,

                ARITHMETIC_NOT =             M_turtle_flag(token::ARITHMETIC_NOT)             | flag::ARITHMETIC,

                ARITHMETIC_GREATER_THAN_EQUAL_TO = (flag::ARITHMETIC_EQUAL_TO | flag::ARITHMETIC_GREATER_THAN),

                ARITHMETIC_LESS_THAN_EQUAL_TO =    (flag::ARITHMETIC_EQUAL_TO | flag::ARITHMETIC_LESS_THAN),
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

    constexpr uint_fast64_t turtleBuiltinTokenMap[][2] = {
        {sti("="), token::flag::DELIMITER_ASSIGN},
        {sti("+"), token::flag::ARITHMETIC_ADD},
        {sti("-"), token::flag::ARITHMETIC_SUB},
        {sti("*"), token::flag::ARITHMETIC_MULL},
        {sti("/"), token::flag::ARITHMETIC_DIV},
        {sti("%"), token::flag::ARITHMETIC_MOD},
        {sti(">"), token::flag::ARITHMETIC_GREATER_THAN},
        {sti("<"), token::flag::ARITHMETIC_LESS_THAN},
        {sti("&"), token::flag::ARITHMETIC_BIT_AND},
        {sti("|"), token::flag::ARITHMETIC_BIT_OR},
        {sti("^"), token::flag::ARITHMETIC_BIT_XOR},
        {sti("~"), token::flag::ARITHMETIC_BIT_NOT},
        {sti("!"), token::flag::ARITHMETIC_NOT},
        {sti("=="), token::flag::ARITHMETIC_EQUAL_TO},
        {sti("!="), token::flag::ARITHMETIC_NOT_EQUAL},
        {sti("//"), token::flag::ARITHMETIC_FLOOR},
        {sti("**"), token::flag::ARITHMETIC_EXPONENTIAL},
        {sti("<<"), token::flag::ARITHMETIC_BIT_LEFT_SHIFT},
        {sti(">>"), token::flag::ARITHMETIC_BIT_LEFT_SHIFT},
        {sti("+="), token::flag::ARITHMETIC_ADD_ASSIGN},
        {sti("-="), token::flag::ARITHMETIC_SUB_ASSIGN},
        {sti("*="), token::flag::ARITHMETIC_MULL_ASSIGN},
        {sti("/="), token::flag::ARITHMETIC_DIV_ASSIGN},
        {sti("%="), token::flag::ARITHMETIC_MOD_ASSIGN},
        {sti(">="), token::flag::ARITHMETIC_GREATER_THAN_EQUAL_TO},
        {sti("<="), token::flag::ARITHMETIC_LESS_THAN_EQUAL_TO},
        {sti("//="), token::flag::ARITHMETIC_FLOOR_ASSIGN},
        {sti("**="), token::flag::ARITHMETIC_EXPONENTIAL_ASSIGN},
        {sti("&="), token::flag::ARITHMETIC_BIT_AND_ASSIGN},
        {sti("|="), token::flag::ARITHMETIC_BIT_OR_ASSIGN},
        {sti("^="), token::flag::ARITHMETIC_BIT_XOR_ASSIGN},
        //"~=" operator does not exist
        {sti("<<="), token::flag::ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN},
        {sti(">>="), token::flag::ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN},

        {sti("False"), token::flag::KEYWORD_FALSE},
        {sti("True"), token::flag::KEYWORD_TRUE},
        {sti("class"), token::flag::KEYWORD_CLASS},
        {sti("finally"), token::flag::KEYWORD_FINALLY},
        {sti("is"), token::flag::KEYWORD_IS},
        {sti("return"), token::flag::KEYWORD_RETURN},
        {sti("None"), token::flag::KEYWORD_NONE},
        {sti("continue"), token::flag::KEYWORD_CONTINUE},
        {sti("for"), token::flag::KEYWORD_FOR},
        {sti("lambda"), token::flag::KEYWORD_LAMBDA},
        {sti("try"), token::flag::KEYWORD_TRY},
        {sti("def"), token::flag::KEYWORD_DEF},
        {sti("from"), token::flag::KEYWORD_FROM},
        {sti("nonlocal"), token::flag::KEYWORD_NONLOCAL},
        {sti("while"), token::flag::KEYWORD_WHILE},
        {sti("and"), token::flag::KEYWORD_AND},
        {sti("del"), token::flag::KEYWORD_DEL},
        {sti("global"), token::flag::KEYWORD_GLOBAL},
        {sti("not"), token::flag::KEYWORD_NOT},
        {sti("with"), token::flag::KEYWORD_WITH},
        {sti("as"), token::flag::KEYWORD_AS},
        {sti("elif"), token::flag::KEYWORD_ELIF},
        {sti("if"), token::flag::KEYWORD_IF},
        {sti("or"), token::flag::KEYWORD_OR},
        {sti("yield"), token::flag::KEYWORD_YIELD},
        {sti("assert"), token::flag::KEYWORD_ASSERT},
        {sti("else"), token::flag::KEYWORD_ELSE},
        {sti("import"), token::flag::KEYWORD_IMPORT},
        {sti("pass"), token::flag::KEYWORD_PASS},
        {sti("break"), token::flag::KEYWORD_BREAK},
        {sti("except"), token::flag::KEYWORD_EXCEPT},
        {sti("in"), token::flag::KEYWORD_IN},
        {sti("raise"), token::flag::KEYWORD_RAISE}};
    //return token flag
    turtle_flag findKeyword(const char *__restrict str)
    {
        if (strlen(str) <= 8)
        {
            const uint_fast64_t hash = sti(str);
            for (uint_fast8_t i = 0; *turtleBuiltinTokenMap[i]; ++i)
            {
                if (hash == *turtleBuiltinTokenMap[i])
                {
                    return turtleBuiltinTokenMap[i][1];
                }
            }
        }
        return 0;
    }
} // namespace turtle

#endif // _TURTLE_TOKEN_H
