#ifndef GRAPH_H
#define GRAPH_H
#include "global.h"
#include "node.hpp"
#include "token.hpp"

void check_braces(turtle::Document &Doc);

void construct_graph(turtle::Document &Doc){

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
        }
    }
    if(braces % 2){
        srand(time(0));
        panic("Unmatched parentheses somewhere in the program%s\n",
              (rand() % 9 == 8 ? ", good luck" : "")
        );
    }
}

/* returns XORed value of the node addresses */
//I am not using a template in this case as then I'd have to
//do ptr_xor<type>(p1,p2) every time I'd declare this
//But In order to prevent code rewriting just use a macro
#define ValueType Node*
ValueType ptr_xor (ValueType ecx, ValueType edx)
{
    return reinterpret_cast<ValueType>(
      reinterpret_cast<uintptr_t>(ecx) ^ reinterpret_cast<uintptr_t>(edx)
    );
}
#undef ValueType

//haaaaaaa
//#define true false

#endif // GRAPH_H
