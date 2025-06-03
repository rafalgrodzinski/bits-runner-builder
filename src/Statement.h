#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "Token.h"
#include "Expression.h"

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
    //shared_ptr<Token> token;
    //shared_ptr<Expression> expression;
    //shared_ptr<Statement> blockStatement;
    //vector<shared_ptr<Statement>> statements;
    //string name;

public:
    Statement(Kind kind);
    //Statement(Kind kind, shared_ptr<Token> token, shared_ptr<Expression> expression, shared_ptr<Statement> blockStatement, vector<shared_ptr<Statement>> statements, string name);
    Kind getKind();
    //shared_ptr<Token> getToken();
    //shared_ptr<Expression> getExpression();
    //shared_ptr<Statement> getBlockStatement();
    //vector<shared_ptr<Statement>> getStatements();
    //string getName();
    bool isValid();
    string toString();
};

class StatementFunctionDeclaration: public Statement {
private:
    string name;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunctionDeclaration(string name, shared_ptr<StatementBlock> statementBlock);
    string getName();
    shared_ptr<StatementBlock> getStatementBlock();
};

class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Statement>> getStatements();
};

class StatementReturn: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementReturn(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
};

class StatementExpression: public Statement {
private:
    shared_ptr<Expression> expression;

public:
    StatementExpression(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
};

class StatementInvalid: public Statement {
//private:
//    string message;

public:
    StatementInvalid();
    //StatementInvalid(string message);
    //string getMessage();
};

#endif