#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Token.h"

class ExpressionInvalid;

class Expression {
public:
    static ExpressionInvalid Invalid;

    virtual std::string toString() = 0;
};

class ExpressionInvalid {

};

/*class ExpressionInteger: Expression {
public:
    ExpressionInteger(Token token);
    std::string toString() override;
};*/

#endif