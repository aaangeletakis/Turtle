#ifndef REGEX_MACROS_DEF_H
#define REGEX_MACROS_DEF_H

#define __BACKSLASH    R"(\\)"
#define __ANY_CHAR     "."
#define __DOUBLEQ      "\""
#define __SINGLEQ      "'"
#define __CARROT       "^"
#define __LSQUAREB     "["
#define __RSQUAREB     "]"
#define __LCURVEDB     "("
#define __RCURVEDB     ")"
#define __ZERO_OR_MORE "*"
#define __ZERO_OR_ONE  "?"
#define __ONE_OR_MORE  "+"
#define __TO           "-"
#define __OR           "|"
#define ___OR
#define ___AND
#define __ELSE(E)           __OR E
#define ___ELSE(E)          __OR __LCURVEDB E __RCURVEDB
#define __ELIF(E)           __OR __LCURVEDB E __RCURVEDB
#define __ONE_OR_MORE_OF(E) E __ONE_OR_MORE
#define __STRING_PREFIX                R"([rRfFUu]{0,2}?)"
#define __ANY_CHAR_THATS_NOT_A_NEWLINE R"([^\r\n])"
#define __NEWLINE                      R"([\n\r])"
#define __WHITESPACE                   R"([ \t])"
#define __ANY(C)                       __LSQUAREB           C __RSQUAREB
#define __ANY_CHAR_OF(C)                    __ANY(C)
#define __NONE_OF(C)                   __LSQUAREB __CARROT  C __RSQUAREB
#define __ZERO_OR_MORE_OF(E)           E __ZERO_OR_MORE
#define __ZERO_OR_ONE_OF(E)            E __ZERO_OR_ONE
//ascii ranges of python symbols
#define __PYTHON_SYMBOLS \
    "!"  __TO R"(\/)" \
    ":"  __TO    "@"  \
 R"(\[)" __TO    "^"  \
    "{"  __TO    "~"
#define __NOCAPTURE_START             "(?:"
#define __NOCAPTURE(E)                  __NOCAPTURE_START E __RCURVEDB
#define __STRING_NOCAPTURE(TYPE)        __NOCAPTURE(__NONE_OF(__BACKSLASH ___OR TYPE) __OR __BACKSLASH ___AND __ANY_CHAR) __ZERO_OR_MORE
#define ___STRING(TYPE) TYPE __STRING_NOCAPTURE(TYPE) TYPE
#define ___DOC_STRING(TYPE) TYPE "{3}" __STRING_NOCAPTURE(TYPE) TYPE "{3}"

#endif //REGEX_MACROS_DEF_H
