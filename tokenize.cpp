//#include "global.h"
#include "node.hpp"
#include <ctre.hpp>

namespace turtle {

void tokenize(std::string &filedata, std::vector<turtle::_Lexeme> &Lexemes)
{
/*
Paste into regex101.com
Replace regex comments with )"${2}R"( by using (\(\?#([^)]*)\))|^

[a-zA-Z]{0,2}?(("|')(\2{2})?)((?:[^\\"]|\\.|\\)*\1)?|(?#   //capture strings - check later on if string
                                                           //                  prefix is valid and the string terminates
)(#[^\r\n]*)|(?#                                           //capture comments
)([\n\r][ \t]*)|(?#                                        //capture newlines
)(\\[^\r\n]*)|(?#                                          //capture \TheBackslashAndAnythingAfterIt
)((?#
    )(\.{3})|(?#                                           //capture ...
    )(->)|(?#                                              //capture ->
                                                           //fucking floating point numbers
    )(\d[\d_]*\.[\d_]*\d[\d_]*[eE]-?[\d_]*)|(?#            //capture exponential floating point literals
    )(\d[\d_]*\.[\d_]*\d[\d_]*[\w]*)|(?#                   //capture floating point literals -> \d.\d [suffix]
    )(\d[\d_]*\.[eE]-?[\d_]*)|(?#                          //capture exponential floating point literals
    )(\d[\d_]*\.\w*)|(?#                                   //capture floating point literals -> \d.   [suffix]
    )(\.\d[\d_]*[eE]-?[\d_]*)|(?#                          //capture exponential floating point literals
    )(\.\d[\d_]*\w*)|(?#                                   //capture floating point literals ->   .\d [suffix]
    )(\d[\d_]*[eE]-?[\d_]*)|(?#                            //capture exponential literals
    )([<>*\/]{2})=?|(?#                                    //capture 2-3 character operators
    )([!%&*+\-<=>@\/\\^|:]=)(?#                            //capture 2 caracter operators
))|(?#
)([!-\/:-@\[-^{-~]|[^\s!-\/:-@\[-^{-~]+)|(?#               //capture anything else
)(\s+)                                                     //capture whitespace in order to keep track of position with ctre
*/
    //when rEgEX is A LaNGUAgE
    static constexpr ctll::fixed_string TokenRegex {
            R"([a-zA-Z]{0,2}?(("|')(\2{2})?)((?:[^\\"]|\\.|\\)*\1)?|)" //capture strings - check later on if string
                                                                       //                  prefix is valid and the string terminates
            R"((#[^\r\n]*)|)"                                          //capture comments
            R"(([\n\r][ \t]*)|)"                                       //capture newlines
            R"((\\[^\r\n]*)|)"                                         //capture \TheBackslashAndAnythingAfterIt
            R"(()"
                R"((\.{3})|)"                                          //capture ...
                R"((->)|)"                                             //capture ->
                                                                       //fucking floating point numbers
                R"((\d[\d_]*\.[\d_]*\d[\d_]*[eE]-?[\d_]*)|)"           //capture exponential floating point literals
                R"((\d[\d_]*\.[\d_]*\d[\d_]*[\w]*)|)"                  //capture floating point literals -> \d.\d [suffix]
                R"((\d[\d_]*\.[eE]-?[\d_]*)|)"                         //capture exponential floating point literals
                R"((\d[\d_]*\.\w*)|)"                                  //capture floating point literals -> \d.   [suffix]
                R"((\.\d[\d_]*[eE]-?[\d_]*)|)"                         //capture exponential floating point literals
                R"((\.\d[\d_]*\w*)|)"                                  //capture floating point literals ->   .\d [suffix]
                R"((\d[\d_]*[eE]-?[\d_]*)|)"                           //capture exponential literals
                R"(([<>*\/]{2})=?|)"                                   //capture 2-3 character operators
                R"(([!%&*+\-<=>@\/\\^|:]=))"                           //capture 2 caracter operators
            R"()|)"
            R"((\{\}|\(\)|\[\])|)"                                     //capture empty braces. Due to the fact that theres nothing in
                                                                       //   them we can combine them as a single token
            R"(([!-\/:-@\[-^{-~]|[^\s!-\/:-@\[-^{-~]+)|)"              //capture anything else
            R"((\s+))"                                                 //capture whitespace in order to keep track of position with ctre
    };
    const auto& matches = ctre::range<TokenRegex>(static_cast<std::string_view>(filedata));
    //std::distance is not constexpr thus it does not work with ctre
    auto distance = [](const auto& first, const auto& last){
        size_t i = 0;
        for(auto it = first; it != last; ++it){
            ++i;
        }
        return i;
    };

    Lexemes.reserve(distance(matches.begin(), matches.end()));



    unsigned int ln = 0, nl_pos = 0;
    uint_fast64_t position = 0;
    for (const auto & match  : matches){
        position += match.size();
        const auto& str = match.str();
        if(str[0] == '\n' || str[0] == '\r'){
            ++ln;
            nl_pos = position;
        }
        if(str[0] != ' '){
            Lexemes.push_back({str, (uint_fast64_t)(position - nl_pos), ln});
        }
    }
}

}
