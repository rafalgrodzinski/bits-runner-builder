#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "Token.h"
#include "Expression.h"

class Expression;

using namespace std;

class Statement;
class StatementBlock;
class StatementReturn;
class StatementExpression;
class StatementInvalid;

class Statement {
public:
    enum Kind {
        FUNCTION_DECLARATION,
        BLOCK,
        RETURN,
        EXPRESSION,
        INVALID
    };

private:
    Kind kind;

public:
    Statement(Kind kind);
    Kind getKind();
    bool isValid();
    virtual string toString();
};

class StatementFunctionDeclaration: public Statement {
private:
    string name;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunctionDeclaration(string name, shared_ptr<StatementBlock> statementBlock);
    string getName();
    shared_ptr<StatementBlock> getStatementBlock();
    string toString() override;
};

class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Statement>> getStatements();
    string toString() override;
};

class StatementReturn: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementReturn(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString() override;
};

class StatementExpression: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementExpression(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString() override;
};

class StatementInvalid: public Statement {
private:
    shared_ptr<Token> token;

public:
    StatementInvalid(shared_ptr<Token> token);
    string toString() override;
};

#endif