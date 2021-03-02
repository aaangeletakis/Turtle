#ifndef TOKENIZE_HPP
#define TOKENIZE_HPP
#include "global.h"
#include "node.hpp"
#include <vector>

namespace turtle {
    void tokenize(std::string &filedata, std::vector<turtle::_Lexeme> &Lexemes);
}

#endif // TOKENIZE_HPP
