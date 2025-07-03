#include "Parser/Expression/Expression.h"

enum class ExpressionBinaryOperation {
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    INVALID
};

class ExpressionBinary: public Expression {
private:
    ExpressionBinaryOperation operation;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;

public:
    ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right);
    ExpressionBinaryOperation getOperation();
    shared_ptr<Expression> getLeft();
    shared_ptr<Expression> getRight();
};