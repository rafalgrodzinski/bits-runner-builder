#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "Token.h"
#include "Expression.h"
#include "Statement.h"

using namespace std;

class Parser {
private:
    vector<Token> tokens;
    int currentIndex = 0;

    shared_ptr<Expression> term(); // +, -
    shared_ptr<Expression> factor(); // *, /, %
    shared_ptr<Expression> primary(); // integer, ()

    shared_ptr<Expression> matchInteger();
    shared_ptr<Expression> matchGrouping();
    shared_ptr<Expression> matchBinary(shared_ptr<Expression> left);

public:
    Parser(vector<Token> tokens);
    //shared_ptr<Expression> getExpression();
    vector<shared_ptr<Statement>> getStatements();
};

#endif