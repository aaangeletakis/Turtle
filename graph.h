#ifndef GRAPH_H
#define GRAPH_H
#include "global.h"
#include "node.hpp"
#include "token.hpp"

void check_braces(turtle::Document &Doc);

void construct_graph(turtle::Document &Doc){
    //check for any missing braces
    //if there are throw an error
    check_braces(Doc);
}

void check_braces(turtle::Document &Doc){
    uint_fast16_t braces = 0;
    for(auto&n : Doc.Nodes){
        switch(n.NodeFlags){
        case turtle::token::flag::Operator::DELIMITER_CURVED_RIGHT_BRACE:
        case turtle::token::flag::Operator::DELIMITER_CURVED_LEFT_BRACE:
        case turtle::token::flag::Operator::DELIMITER_SQUARE_RIGHT_BRACE:
        case turtle::token::flag::Operator::DELIMITER_SQUARE_LEFT_BRACE:
        case turtle::token::flag::Operator::DELIMITER_CURLY_RIGHT_BRACE:
        case turtle::token::flag::Operator::DELIMITER_CURLY_LEFT_BRACE:
            ++braces;
            break;
        default: break;
        }
    }
    if(braces & 1){
        srand(time(0));
        panic("Unmatched parentheses somewhere in the program%s\n",
              (rand() % 9 == 8 ? ", good luck" : "")
        );
    }
}

#endif // GRAPH_H
