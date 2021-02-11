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
    const uint64_t maxlines     = Doc.Nodes.back().line + 1;
          uint64_t currline     = 0;
          uint64_t lineindex    = 0;
          uint64_t tokensOnLine = 0;
    Doc.Graph.reserve(maxlines + 1);
    for(const auto& n: Doc.Nodes){
        if(n.line == currline){
            ++tokensOnLine;
        } else {
            currline = n.line;
            Node line_node;
            line_node.NodeFlags = turtle::token::flag::Control::NEWLINE;
            line_node.children.reserve(tokensOnLine + 1);
            Doc.Graph.push_back(std::move(line_node));
            tokensOnLine = 0;
        }
    }
    currline     = 0;
    lineindex    = 0;
    {
        Node tmpNode;
        tmpNode.NodeFlags = turtle::token::flag::Control::ENDMARKER;
        Doc.Graph.push_back(std::move(tmpNode));
    }
    for(auto& n: Doc.Nodes){
        if(n.line == currline){
            Doc.Graph[lineindex].children.push_back(&n);
        } else {
            currline = n.line;
            Doc.Graph[lineindex].children.push_back(&Doc.Graph.back());
            ++lineindex;
        }
    }
    for(auto& line : Doc.Graph){
       const auto& flag_str = turtle::getFlagName(line.NodeFlags);
       std::cout << flag_str << flag_str.size() + static_cast<const char *>("         ") <<" (";
        for(auto& n : line.children){
            std::cout << '[' << turtle::getFlagName(n->NodeFlags) << ']';
        }
       std::cout << ")\n";
    }
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

#endif // GRAPH_H
