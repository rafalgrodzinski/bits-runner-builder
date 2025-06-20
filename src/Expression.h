#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Token.h"
#include "Statement.h"
#include "Types.h"

class StatementBlock;
class StatementExpression;

using namespace std;

enum class ExpressionKind {
    LITERAL,
    GROUPING,
    BINARY,
    IF_ELSE,
    VAR,
    CALL,
    INVALID
};

//
// Expression
class Expression {
private:
    ExpressionKind kind;

protected:
    ValueType valueType;

public:
    Expression(ExpressionKind kind, ValueType valueType);
    ExpressionKind getKind();
    ValueType getValueType();
    bool isValid();
    virtual string toString(int indent);
};

//
// ExpressionLiteral
class ExpressionLiteral: public Expression {
private:
    bool boolValue;
    int32_t sint32Value;
    float real32Value;

public:
    ExpressionLiteral(shared_ptr<Token> token);
    bool getBoolValue();
    int32_t getSint32Value();
    float getReal32Value();
    string toString(int indent) override;
};

//
// ExpressionGrouping
class ExpressionGrouping: public Expression {
private:
    shared_ptr<Expression> expression;

public:
    ExpressionGrouping(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};

//
// ExpressionBinary
class ExpressionBinary: public Expression {
public:
    enum Operation {
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
        MOD
    };

private:
    Operation operation;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;

public:
    ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right);
    Operation getOperation();
    shared_ptr<Expression> getLeft();
    shared_ptr<Expression> getRight();
    string toString(int indent) override;
};

//
// ExpressionIfElse
class ExpressionIfElse: public Expression {
private:
    shared_ptr<Expression> condition;
    shared_ptr<StatementBlock> thenBlock;
    shared_ptr<StatementBlock> elseBlock;

public:
    ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<StatementBlock> thenBlock, shared_ptr<StatementBlock> elseBlock);
    shared_ptr<Expression> getCondition();
    shared_ptr<StatementBlock> getThenBlock();
    shared_ptr<StatementBlock> getElseBlock();
    string toString(int indent) override;
};

//
// ExpressionVar
class ExpressionVar: public Expression {
private:
    string name;

public:
    ExpressionVar(string name);
    string getName();
    string toString(int indent) override;
};

//
// Expression Call
class ExpressionCall: public Expression {
private:
    string name;
    vector<shared_ptr<Expression>> argumentExpressions;

public:
    ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions);
    string getName();
    vector<shared_ptr<Expression>> getArgumentExpressions();
    string toString(int indent) override;
};

//
// ExpressionInvalid
class ExpressionInvalid: public Expression {
private:
    shared_ptr<Token> token;

public:
    ExpressionInvalid(shared_ptr<Token> token);
    shared_ptr<Token> getToken();
    string toString(int indent) override;
};

#endif