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
 * The use of enums are really great as they can be
 * used for pre-calculated magic numbers
 *
 * Currently the the token flag is represented by a 32 bit unsigned integer.
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

//#include <string>
#include <stdint.h>
#include <string.h>
#include "global.h"


#if DEBUG_CPP
  #include "enum.h"
#else
  #define BETTER_ENUM(ENAME, TYPE, ...) \
        namespace  ENAME {              \
            enum {                      \
                __VA_ARGS__             \
            };                          \
        }
#endif


#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/any.hpp>

namespace turtle {
    //In case I want to change it to something bigger in the future
    typedef uint_fast32_t turtle_flag;

    typedef boost::multiprecision::cpp_int          turtle_int;
    typedef boost::multiprecision::cpp_dec_float_50 turtle_float;
    typedef boost::any                              turtle_any;
    using   boost::any_cast;

    typedef turtle_int                              t_int;
    typedef turtle_float                            t_float;
    typedef turtle_any                              t_any;
}

//      M_turtle_flag(N) (00000000 00000000 00000000 00000001 <<  N )
#define M_turtle_flag(N) ((turtle_flag)1 << (N))

//make constexpr in order to reduce compile time
constexpr auto tokenTypeOffset = ( ((sizeof(turtle::turtle_flag) * 8) - (3 /* Number of Bits needed */)) );

//Refer to the huge comment in the flag namespace on wtf this is & does
#define M_typeFlagMacro(N) ((turtle_flag)N << tokenTypeOffset)

struct Node
{
    enum {NEXT, LAST};
    //Node
    turtle::turtle_flag NodeFlags = 0;
    Node *node[2] = {0};
    uint_fast16_t linepos = 0;
    uint_fast16_t line = 0;
    inline constexpr auto type()
    {
        return NodeFlags >> tokenTypeOffset;
    }
    inline constexpr bool test_bit(unsigned char i){
        return !!(NodeFlags & ((turtle::turtle_flag)1<<i));
    }
    //extract bits then check flag tegrety
    inline constexpr bool hasFlag(turtle::turtle_flag __f){
        return (NodeFlags & __f) == __f;
    }
    inline constexpr bool hasType(turtle::turtle_flag __f){
        return (NodeFlags >> tokenTypeOffset) == __f;
    }
};

namespace turtle
{
    //std::vector<struct Node> SyntaxGroups
    //         ||
    //         \/
    //std::vector<struct Node> SemanticGroups

#if DEBUG_CPP
    template <class T>
    constexpr const char * flagstr(T values, turtle_flag flag){
        for (const auto& v : values) {
            if(v._to_integral() == flag){
                return v._to_string();
            }
        }
        return "";
    }
#endif

    //extract bits then check flag integrety
    inline constexpr bool hasFlag(turtle_flag src, turtle_flag __f){
        return ((src & __f) == __f);
    }
    namespace token
    {

        #define __ENUM_NAME Type
        BETTER_ENUM(__ENUM_NAME, turtle_flag,
            CONTROL,
            DELIMITERS, // such as '(' or ')' or '.' or '[' or ']' ','
            ARITHMETIC,
            KEYWORD,    // any builtin type
            DATA,       // such as a number or string
            IDENTIFIER//, // any label
            //NUMBER_OF_BUILTIN_TYPES
        )
        #undef __ENUM_NAME

        #define __ENUM_NAME Control
        BETTER_ENUM(__ENUM_NAME, turtle_flag,
            NULL_TOKEN,
            HAS_VALUE = NULL_TOKEN,
            NEWLINE
        )
        #undef __ENUM_NAME

        #define __ENUM_NAME Data
        BETTER_ENUM(__ENUM_NAME, turtle_flag,
            DATA_TYPE_STRING,
                DATA_TYPE_RAW, // Used in combination with the string flag
                DATA_TYPE_FORMATTED,
                DATA_TYPE_FORMAT_TYPE, // "%s" printf style or "{}".format style?
                DATA_TYPE_UNICODE,

            DATA_TYPE_NUMBER,
                    DATA_TYPE_INT,
                    DATA_TYPE_FLOAT,
                    DATA_TYPE_COMPLEX,
                    DATA_TYPE_EXPONENTIAL,
                    DATA_TYPE_HEX,
                    DATA_TYPE_OCTAL,
            DATA_TYPE_, //unknown -- make effort to determine type
            DATA_TYPE_COMMENT
        )
        #undef /*cuz its*/__ENUM_NAME //sun

        #define __ENUM_NAME Operator
        BETTER_ENUM(__ENUM_NAME, turtle_flag,
            DELIMITER_AT_SIGN, // '@' Python decorator, akin to passing a function pointer
            DELIMITER_COLON,   // ':' symbol
            DELIMITER_SEMICOLON,
            DELIMITER_COMMA,
            DELIMITER_PERIOD, // access token '.'
            DELIMITER_BRACE,
            //DELIMITER_ACCESS,

            DELIMITER_LEFT_OR_RIGHT_BRACE = DELIMITER_BRACE + 1, // 0 = Left, 1 = Right
            DELIMITER_CURVED_BRACE,                              // '(' or ')'
            DELIMITER_SQUARE_BRACE,                              // '[' or ']'
            DELIMITER_CURLY_BRACE,                               // '{' or '}'
            DELIMITER_ASSIGN // '=' symbol
        )
        #undef __ENUM_NAME

        //like identifiers these will be represented as integers
        #define __ENUM_NAME Keyword
        BETTER_ENUM(__ENUM_NAME, turtle_flag,
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
            KEYWORD_RAISE
        )
        #undef __ENUM_NAME

        #define __ENUM_NAME Arithmetic
        BETTER_ENUM(__ENUM_NAME, turtle_flag,
            ARITHMETIC_OPERATION = 0, // 1 = ARITHMETIC, 0 = LOGICAL (greater than, less than, equal to, not)

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
            ARITHMETIC_NOT//,

            //ARITHMETIC_NOT_EQUAL_TO,

            //ARITHMETIC_GREATER_THAN_EQUAL_TO,

            //ARITHMETIC_LESS_THAN_EQUAL_TO,
        )
        #undef __ENUM_NAME

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
             */
            #define __ENUM_NAME Type
            BETTER_ENUM(__ENUM_NAME, turtle_flag,
                CONTROL =    M_typeFlagMacro(token::__ENUM_NAME::CONTROL),
                DELIMITERS = M_typeFlagMacro(token::__ENUM_NAME::DELIMITERS),
                ARITHMETIC = M_typeFlagMacro(token::__ENUM_NAME::ARITHMETIC),
                KEYWORD =    M_typeFlagMacro(token::__ENUM_NAME::KEYWORD),
                DATA =       M_typeFlagMacro(token::__ENUM_NAME::DATA),
                IDENTIFIER = M_typeFlagMacro(token::__ENUM_NAME::IDENTIFIER)
            )
            #undef __ENUM_NAME
            /*
             *
             *   ┌──> Flag Type - Control Class Id
             *   │
             * ┌─┤┌───> Is null token
             * 00000000  00000000 00000000 00000000
             *
             *
             *
             *
             *   ┌──> Flag Type - Control Class Id
             *   │
             * ┌─┤┌───> Is not null
             * 00011111  11111111 11111111 11111111         
             *     │└─────────────────────────────┴──> Amount Of whitespace
             *     └────> Is newline
             */
            #define control_type_macro(N) ( (N) << (tokenTypeOffset - 3) )
            #define __ENUM_NAME Control
            BETTER_ENUM(__ENUM_NAME, turtle_flag,
                NULL_TOKEN = 0 | flag::Type::CONTROL,
                NEWLINE =    control_type_macro(M_turtle_flag(token::__ENUM_NAME::NEWLINE) | M_turtle_flag(token::__ENUM_NAME::HAS_VALUE) | flag::Type::CONTROL)
            )
            #undef __ENUM_NAME

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

               //Strings
                  ┌──> Flag Type - DATA Class Id
                  │
                ┌─┤
                10000001  00000000 00000000 00000000
                   ││││└──> Is string
                   │││└───> Is raw string
                   ││└────> Is formated string
                   │└─────> Format Type
                   └──────> Is unicode string
             */
            #define DataShiftToMargin(N) ( (N) << (tokenTypeOffset - 12) )
            #define __ENUM_NAME Data
            BETTER_ENUM(__ENUM_NAME,  turtle_flag,
                DATA_TYPE_STRING =     DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_STRING))        | flag::Type::DATA,
                DATA_TYPE_RAW =        DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_RAW))           | flag::Type::DATA,
                DATA_TYPE_FORMATTED =  DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_FORMATTED))     | flag::Type::DATA,
                PRINTF_STYLE_FORMAT =  DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_FORMAT_TYPE))   | flag::Type::DATA,
                DATA_TYPE_UNICODE =    DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_UNICODE))       | flag::Type::DATA,

                DATA_TYPE_RAW_STRING =             (flag::__ENUM_NAME::DATA_TYPE_RAW             | flag::__ENUM_NAME::DATA_TYPE_STRING),
                DATA_TYPE_FORMATED_STRING =        (flag::__ENUM_NAME::DATA_TYPE_FORMATTED       | flag::__ENUM_NAME::DATA_TYPE_STRING),
                DATA_TYPE_FORMATED_PRINTF_STRING = (flag::__ENUM_NAME::PRINTF_STYLE_FORMAT       | flag::__ENUM_NAME::DATA_TYPE_FORMATED_STRING),
                DATA_TYPE_FORMATED_PYTHON_STRING = DATA_TYPE_FORMATED_STRING,
                DATA_TYPE_UNICODE_STRING =         (flag::__ENUM_NAME::DATA_TYPE_UNICODE         | flag::__ENUM_NAME::DATA_TYPE_STRING),
                DATA_TYPE_FORMATED_UNICODE_STRING = (flag::__ENUM_NAME::DATA_TYPE_FORMATED_STRING | flag::__ENUM_NAME::DATA_TYPE_UNICODE_STRING),

                DATA_TYPE_COMMENT =    DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_COMMENT))       | flag::Type::DATA,
                DATA_TYPE_NUMBER =     DataShiftToMargin(M_turtle_flag(token::__ENUM_NAME::DATA_TYPE_NUMBER))        | flag::Type::DATA
            )
            #undef __ENUM_NAME
            #undef DataShiftToMargin

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


            #define __ENUM_NAME Operator
            //offset the arithmetic tokens to be next to the DELIMITER_ASSIGN token
            #define DeliminarAssignOffset_M(x) (x << (token::__ENUM_NAME::DELIMITER_ASSIGN + 1))
            BETTER_ENUM(__ENUM_NAME, turtle_flag,
                DELIMITER_ASSIGN =                 M_turtle_flag(token::__ENUM_NAME::DELIMITER_ASSIGN) | flag::Type::DELIMITERS,

                ARITHMETIC_ADD_ASSIGN =             DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_ADD)             | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_SUB_ASSIGN =             DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_SUB)             | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_MULL_ASSIGN =            DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_MULL)            | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_DIV_ASSIGN =             DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_DIV)             | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_MOD_ASSIGN =             DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_MOD)             | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_FLOOR_ASSIGN =           DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_FLOOR)           | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_EXPONENTIAL_ASSIGN =     DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_EXPONENTIAL)     | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_BIT_AND_ASSIGN =         DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_AND)         | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_BIT_OR_ASSIGN =          DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_OR)          | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_BIT_XOR_ASSIGN =         DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_XOR)         | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN =  DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_LEFT_SHIFT)  | flag::__ENUM_NAME::DELIMITER_ASSIGN,
                ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN = DeliminarAssignOffset_M(token::Arithmetic::ARITHMETIC_BIT_RIGHT_SHIFT) | flag::__ENUM_NAME::DELIMITER_ASSIGN,

                DELIMITER_AT_SIGN =                 M_turtle_flag(token::__ENUM_NAME::DELIMITER_AT_SIGN)                    | flag::Type::DELIMITERS,
                DELIMITER_COLON =                   M_turtle_flag(token::__ENUM_NAME::DELIMITER_COLON)                      | flag::Type::DELIMITERS,
                DELIMITER_SEMICOLON =               M_turtle_flag(token::__ENUM_NAME::DELIMITER_SEMICOLON)                  | flag::Type::DELIMITERS,
                DELIMITER_COMMA =                   M_turtle_flag(token::__ENUM_NAME::DELIMITER_COMMA)                      | flag::Type::DELIMITERS,
                DELIMITER_PERIOD =                  M_turtle_flag(token::__ENUM_NAME::DELIMITER_PERIOD)                     | flag::Type::DELIMITERS,
                DELIMITER_ACCESS =                                                                               DELIMITER_PERIOD,

                DELIMITER_BRACE =                   M_turtle_flag(token::__ENUM_NAME::DELIMITER_BRACE)                      | flag::Type::DELIMITERS,
                DELIMITER_LEFT_BRACE =              M_turtle_flag(token::__ENUM_NAME::DELIMITER_LEFT_OR_RIGHT_BRACE)        | flag::Type::DELIMITERS, // 1 = Left, 0 = Right

                DELIMITER_CURVED_BRACE =            M_turtle_flag(token::__ENUM_NAME::DELIMITER_CURVED_BRACE)               | flag::Type::DELIMITERS,
                DELIMITER_SQUARE_BRACE =            M_turtle_flag(token::__ENUM_NAME::DELIMITER_SQUARE_BRACE)               | flag::Type::DELIMITERS,
                DELIMITER_CURLY_BRACE =             M_turtle_flag(token::__ENUM_NAME::DELIMITER_CURLY_BRACE)                | flag::Type::DELIMITERS,

                DELIMITER_CURVED_RIGHT_BRACE =      (DELIMITER_CURVED_BRACE                        | DELIMITER_BRACE),
                DELIMITER_CURVED_LEFT_BRACE =       (DELIMITER_CURVED_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
                DELIMITER_SQUARE_RIGHT_BRACE =      (DELIMITER_SQUARE_BRACE                        | DELIMITER_BRACE),
                DELIMITER_SQUARE_LEFT_BRACE =       (DELIMITER_SQUARE_BRACE | DELIMITER_LEFT_BRACE | DELIMITER_BRACE),
                DELIMITER_CURLY_RIGHT_BRACE =       (DELIMITER_CURLY_BRACE                         | DELIMITER_BRACE),
                DELIMITER_CURLY_LEFT_BRACE =        (DELIMITER_CURLY_BRACE  | DELIMITER_LEFT_BRACE | DELIMITER_BRACE)
            )
        #undef DeliminarAssignOffset_M
        #undef __ENUM_NAME

        #define __ENUM_NAME Keyword
            BETTER_ENUM(__ENUM_NAME, turtle_flag,
                KEYWORD_FALSE =      token::__ENUM_NAME::KEYWORD_FALSE    | flag::Type::KEYWORD,
                KEYWORD_CLASS =      token::__ENUM_NAME::KEYWORD_CLASS    | flag::Type::KEYWORD,
                KEYWORD_FINALLY =    token::__ENUM_NAME::KEYWORD_FINALLY  | flag::Type::KEYWORD,
                KEYWORD_IS =         token::__ENUM_NAME::KEYWORD_IS       | flag::Type::KEYWORD,
                KEYWORD_RETURN =     token::__ENUM_NAME::KEYWORD_RETURN   | flag::Type::KEYWORD,
                KEYWORD_NONE =       token::__ENUM_NAME::KEYWORD_NONE     | flag::Type::KEYWORD,
                KEYWORD_CONTINUE =   token::__ENUM_NAME::KEYWORD_CONTINUE | flag::Type::KEYWORD,
                KEYWORD_FOR =        token::__ENUM_NAME::KEYWORD_FOR      | flag::Type::KEYWORD,
                KEYWORD_LAMBDA =     token::__ENUM_NAME::KEYWORD_LAMBDA   | flag::Type::KEYWORD,
                KEYWORD_TRY =        token::__ENUM_NAME::KEYWORD_TRY      | flag::Type::KEYWORD,
                KEYWORD_TRUE =       token::__ENUM_NAME::KEYWORD_TRUE     | flag::Type::KEYWORD,
                KEYWORD_DEF =        token::__ENUM_NAME::KEYWORD_DEF      | flag::Type::KEYWORD,
                KEYWORD_FROM =       token::__ENUM_NAME::KEYWORD_FROM     | flag::Type::KEYWORD,
                KEYWORD_NONLOCAL =   token::__ENUM_NAME::KEYWORD_NONLOCAL | flag::Type::KEYWORD,
                KEYWORD_WHILE =      token::__ENUM_NAME::KEYWORD_WHILE    | flag::Type::KEYWORD,
                KEYWORD_AND =        token::__ENUM_NAME::KEYWORD_AND      | flag::Type::KEYWORD,
                KEYWORD_DEL =        token::__ENUM_NAME::KEYWORD_DEL      | flag::Type::KEYWORD,
                KEYWORD_GLOBAL =     token::__ENUM_NAME::KEYWORD_GLOBAL   | flag::Type::KEYWORD,
                KEYWORD_NOT =        token::__ENUM_NAME::KEYWORD_NOT      | flag::Type::KEYWORD,
                KEYWORD_WITH =       token::__ENUM_NAME::KEYWORD_WITH     | flag::Type::KEYWORD,
                KEYWORD_AS =         token::__ENUM_NAME::KEYWORD_AS       | flag::Type::KEYWORD,
                KEYWORD_ELIF =       token::__ENUM_NAME::KEYWORD_ELIF     | flag::Type::KEYWORD,
                KEYWORD_IF =         token::__ENUM_NAME::KEYWORD_IF       | flag::Type::KEYWORD,
                KEYWORD_OR =         token::__ENUM_NAME::KEYWORD_OR       | flag::Type::KEYWORD,
                KEYWORD_YIELD =      token::__ENUM_NAME::KEYWORD_YIELD    | flag::Type::KEYWORD,
                KEYWORD_ASSERT =     token::__ENUM_NAME::KEYWORD_ASSERT   | flag::Type::KEYWORD,
                KEYWORD_ELSE =       token::__ENUM_NAME::KEYWORD_ELSE     | flag::Type::KEYWORD,
                KEYWORD_IMPORT =     token::__ENUM_NAME::KEYWORD_IMPORT   | flag::Type::KEYWORD,
                KEYWORD_PASS =       token::__ENUM_NAME::KEYWORD_PASS     | flag::Type::KEYWORD,
                KEYWORD_BREAK =      token::__ENUM_NAME::KEYWORD_BREAK    | flag::Type::KEYWORD,
                KEYWORD_EXCEPT =     token::__ENUM_NAME::KEYWORD_EXCEPT   | flag::Type::KEYWORD,
                KEYWORD_IN =         token::__ENUM_NAME::KEYWORD_IN       | flag::Type::KEYWORD,
                KEYWORD_RAISE =      token::__ENUM_NAME::KEYWORD_RAISE    | flag::Type::KEYWORD
            )
        #undef __ENUM_NAME

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
        #define __ENUM_NAME Arithmetic
            BETTER_ENUM(__ENUM_NAME, turtle_flag,
                ARITHMETIC_OPERATION =             M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_OPERATION)       | flag::Type::ARITHMETIC,

                ARITHMETIC_ADD =                   M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_ADD)             | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_SUB =                   M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_SUB)             | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_MULL =                  M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_MULL)            | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_DIV =                   M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_DIV)             | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_MOD =                   M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_MOD)             | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_FLOOR =                 M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_FLOOR)           | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_EXPONENTIAL =           M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_EXPONENTIAL)     | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_AND =               M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_BIT_AND)         | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_OR =                M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_BIT_OR)          | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_XOR =               M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_BIT_XOR)         | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_NOT =               M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_BIT_NOT)         | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_LEFT_SHIFT =        M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_BIT_LEFT_SHIFT)  | flag::__ENUM_NAME::ARITHMETIC_OPERATION,
                ARITHMETIC_BIT_RIGHT_SHIFT =       M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_BIT_RIGHT_SHIFT) | flag::__ENUM_NAME::ARITHMETIC_OPERATION,

                ARITHMETIC_EQUAL_TO =              M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_EQUAL_TO)        | flag::Type::ARITHMETIC,

                ARITHMETIC_NOT_EQUAL =             M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_NOT)             | flag::__ENUM_NAME::ARITHMETIC_EQUAL_TO,

                ARITHMETIC_GREATER_THAN =          M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_GREATER_THAN)    | flag::Type::ARITHMETIC,

                ARITHMETIC_LESS_THAN =             M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_LESS_THAN)       | flag::Type::ARITHMETIC,

                ARITHMETIC_NOT =                   M_turtle_flag(token::__ENUM_NAME::ARITHMETIC_NOT)             | flag::Type::ARITHMETIC,

                ARITHMETIC_GREATER_THAN_EQUAL_TO = (flag::__ENUM_NAME::ARITHMETIC_EQUAL_TO | flag::__ENUM_NAME::ARITHMETIC_GREATER_THAN),

                ARITHMETIC_LESS_THAN_EQUAL_TO =    (flag::__ENUM_NAME::ARITHMETIC_EQUAL_TO | flag::__ENUM_NAME::ARITHMETIC_LESS_THAN)
            )
        #undef __ENUM_NAME
        } // namespace flag

    } // namespace token

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

    /*
     * Unfortunately std::map can not be constexpr-ed
     * Although memory is cheap, the less used the better
     * And it is my opinion to allocations on the stack make
     * for a safer garentee that the program will run effectively,
     * efficently, and fast
    */
    constexpr uint_fast64_t turtleBuiltinTokenMap[][2] = {
            {sti(","),        token::flag::Operator::   DELIMITER_COMMA},
            {sti(";"),        token::flag::Operator::   DELIMITER_SEMICOLON},
            {sti(":"),        token::flag::Operator::   DELIMITER_COLON},
            {sti("("),        token::flag::Operator::   DELIMITER_CURVED_LEFT_BRACE},
            {sti(")"),        token::flag::Operator::   DELIMITER_CURVED_RIGHT_BRACE},
            {sti("{"),        token::flag::Operator::   DELIMITER_CURLY_LEFT_BRACE},
            {sti("}"),        token::flag::Operator::   DELIMITER_CURLY_RIGHT_BRACE},
            {sti("["),        token::flag::Operator::   DELIMITER_SQUARE_LEFT_BRACE},
            {sti("]"),        token::flag::Operator::   DELIMITER_SQUARE_RIGHT_BRACE},
            {sti("."),        token::flag::Operator::   DELIMITER_PERIOD},
            {sti("="),        token::flag::Operator::   DELIMITER_ASSIGN},
            {sti("+"),        token::flag::Arithmetic:: ARITHMETIC_ADD},
            {sti("-"),        token::flag::Arithmetic:: ARITHMETIC_SUB},
            {sti("*"),        token::flag::Arithmetic:: ARITHMETIC_MULL},
            {sti("/"),        token::flag::Arithmetic:: ARITHMETIC_DIV},
            {sti("%"),        token::flag::Arithmetic:: ARITHMETIC_MOD},
            {sti(">"),        token::flag::Arithmetic:: ARITHMETIC_GREATER_THAN},
            {sti("<"),        token::flag::Arithmetic:: ARITHMETIC_LESS_THAN},
            {sti("&"),        token::flag::Arithmetic:: ARITHMETIC_BIT_AND},
            {sti("|"),        token::flag::Arithmetic:: ARITHMETIC_BIT_OR},
            {sti("^"),        token::flag::Arithmetic:: ARITHMETIC_BIT_XOR},
            {sti("~"),        token::flag::Arithmetic:: ARITHMETIC_BIT_NOT},
            {sti("!"),        token::flag::Arithmetic:: ARITHMETIC_NOT},
            {sti("=="),       token::flag::Arithmetic:: ARITHMETIC_EQUAL_TO},
            {sti("!="),       token::flag::Arithmetic:: ARITHMETIC_NOT_EQUAL},
            {sti("//"),       token::flag::Arithmetic:: ARITHMETIC_FLOOR},
            {sti("**"),       token::flag::Arithmetic:: ARITHMETIC_EXPONENTIAL},
            {sti("<<"),       token::flag::Arithmetic:: ARITHMETIC_BIT_LEFT_SHIFT},
            {sti(">>"),       token::flag::Arithmetic:: ARITHMETIC_BIT_LEFT_SHIFT},
            {sti("+="),       token::flag::Operator::   ARITHMETIC_ADD_ASSIGN},
            {sti("-="),       token::flag::Operator::   ARITHMETIC_SUB_ASSIGN},
            {sti("*="),       token::flag::Operator::   ARITHMETIC_MULL_ASSIGN},
            {sti("/="),       token::flag::Operator::   ARITHMETIC_DIV_ASSIGN},
            {sti("%="),       token::flag::Operator::   ARITHMETIC_MOD_ASSIGN},
            {sti(">="),       token::flag::Arithmetic:: ARITHMETIC_GREATER_THAN_EQUAL_TO},
            {sti("<="),       token::flag::Arithmetic:: ARITHMETIC_LESS_THAN_EQUAL_TO},
            {sti("//="),      token::flag::Operator::   ARITHMETIC_FLOOR_ASSIGN},
            {sti("**="),      token::flag::Operator::   ARITHMETIC_EXPONENTIAL_ASSIGN},
            {sti("&="),       token::flag::Operator::   ARITHMETIC_BIT_AND_ASSIGN},
            {sti("|="),       token::flag::Operator::   ARITHMETIC_BIT_OR_ASSIGN},
            {sti("^="),       token::flag::Operator::   ARITHMETIC_BIT_XOR_ASSIGN},

            //"~=" operator does not exist

            {sti("<<="),      token::flag::Operator::   ARITHMETIC_BIT_LEFT_SHIFT_ASSIGN},
            {sti(">>="),      token::flag::Operator::   ARITHMETIC_BIT_RIGHT_SHIFT_ASSIGN},

            {sti("False"),    token::flag::Keyword::    KEYWORD_FALSE},
            {sti("True"),     token::flag::Keyword::    KEYWORD_TRUE},
            {sti("class"),    token::flag::Keyword::    KEYWORD_CLASS},
            {sti("finally"),  token::flag::Keyword::    KEYWORD_FINALLY},
            {sti("is"),       token::flag::Keyword::    KEYWORD_IS},
            {sti("return"),   token::flag::Keyword::    KEYWORD_RETURN},
            {sti("None"),     token::flag::Keyword::    KEYWORD_NONE},
            {sti("continue"), token::flag::Keyword::    KEYWORD_CONTINUE},
            {sti("for"),      token::flag::Keyword::    KEYWORD_FOR},
            {sti("lambda"),   token::flag::Keyword::    KEYWORD_LAMBDA},
            {sti("try"),      token::flag::Keyword::    KEYWORD_TRY},
            {sti("def"),      token::flag::Keyword::    KEYWORD_DEF},
            {sti("from"),     token::flag::Keyword::    KEYWORD_FROM},
            {sti("nonlocal"), token::flag::Keyword::    KEYWORD_NONLOCAL},
            {sti("while"),    token::flag::Keyword::    KEYWORD_WHILE},
            {sti("and"),      token::flag::Keyword::    KEYWORD_AND},
            {sti("del"),      token::flag::Keyword::    KEYWORD_DEL},
            {sti("global"),   token::flag::Keyword::    KEYWORD_GLOBAL},
            {sti("not"),      token::flag::Keyword::    KEYWORD_NOT},
            {sti("with"),     token::flag::Keyword::    KEYWORD_WITH},
            {sti("as"),       token::flag::Keyword::    KEYWORD_AS},
            {sti("elif"),     token::flag::Keyword::    KEYWORD_ELIF},
            {sti("if"),       token::flag::Keyword::    KEYWORD_IF},
            {sti("or"),       token::flag::Keyword::    KEYWORD_OR},
            {sti("yield"),    token::flag::Keyword::    KEYWORD_YIELD},
            {sti("assert"),   token::flag::Keyword::    KEYWORD_ASSERT},
            {sti("else"),     token::flag::Keyword::    KEYWORD_ELSE},
            {sti("import"),   token::flag::Keyword::    KEYWORD_IMPORT},
            {sti("pass"),     token::flag::Keyword::    KEYWORD_PASS},
            {sti("break"),    token::flag::Keyword::    KEYWORD_BREAK},
            {sti("except"),   token::flag::Keyword::    KEYWORD_EXCEPT},
            {sti("in"),       token::flag::Keyword::    KEYWORD_IN},
            {sti("raise"),    token::flag::Keyword::    KEYWORD_RAISE}
        };
    //return token flag
    turtle_flag findToken(const char *__restrict str)
    {
        if (strlen(str) <= 8)
        {
            const uint_fast64_t hash = sti(str);
            for (uint_fast8_t i = 0; *turtleBuiltinTokenMap[i]; ++i)
            {
                if (hash == turtleBuiltinTokenMap[i][0])
                {
                    return  turtleBuiltinTokenMap[i][1];
                }
            }
        }
        return token::flag::Control::NULL_TOKEN;
    }
} // namespace turtle

#endif // _TURTLE_TOKEN_H
