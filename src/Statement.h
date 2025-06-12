#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "Token.h"
#include "Expression.h"
#include "Types.h"

class Expression;
class Statement;
class StatementBlock;
class StatementReturn;
class StatementExpression;
class StatementInvalid;

using namespace std;

//
// Statement
class Statement {
private:
    StatementKind kind;

public:
    Statement(StatementKind kind);
    StatementKind getKind();
    bool isValid();
    virtual string toString(int indent);
};

//
// StatementFunctionDeclaration
class StatementFunctionDeclaration: public Statement {
private:
    string name;
    ValueType returnValueType;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunctionDeclaration(string name, ValueType returnValueType, shared_ptr<StatementBlock> statementBlock);
    string getName();
    ValueType getReturnValueType();
    shared_ptr<StatementBlock> getStatementBlock();
    string toString(int indent) override;
};

//
// StatementVarDeclaration
class StatementVarDeclaration: public Statement {
private:
    string name;
    ValueType valueType;
    shared_ptr<Expression> expression;

public:
    StatementVarDeclaration(string name, ValueType valueType, shared_ptr<Expression> expression);
    string getName();
    ValueType getValueType();
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};

//
// StatementBlock
class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;
    shared_ptr<StatementExpression> statementExpression;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Statement>> getStatements();
    shared_ptr<StatementExpression> getStatementExpression();
    string toString(int indent) override;
};

//
// StatementReturn
class StatementReturn: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementReturn(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};

//
// StatementExpression
class StatementExpression: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementExpression(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};

//
// StatementInvalid
class StatementInvalid: public Statement {
private:
    shared_ptr<Token> token;
    string message;

public:
    StatementInvalid(shared_ptr<Token> token, string message);
    string toString(int indent) override;
    string getMessage();
};

#endif