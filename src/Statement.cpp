#include "Statement.h"

/*Statement::Statement(Kind kind, shared_ptr<Token> token, shared_ptr<Expression> expression, shared_ptr<Statement> blockStatement, vector<shared_ptr<Statement>> statements, string name):
    kind(kind), token(token), expression(expression), blockStatement(blockStatement), statements(statements), name(name) {
}*/
Statement::Statement(Kind kind): kind(kind) {
}

Statement::Kind Statement::getKind() {
    return kind;
}

/*shared_ptr<Token> Statement::getToken() {
    return token;
}*/

/*shared_ptr<Expression> Statement::getExpression() {
    return expression;
}*/

/*shared_ptr<Statement> Statement::getBlockStatement() {
    return blockStatement;
}*/

/*vector<shared_ptr<Statement>> Statement::getStatements() {
    return statements;
}*/

/*string Statement::getName() {
    return name;
}*/

bool Statement::isValid() {
    return kind != Statement::Kind::INVALID;
}

string Statement::toString() {
    /*switch (kind) {
        case EXPRESSION:
            return expression->toString();
        case BLOCK: {
            string value;
            for (int i=0; i<statements.size(); i++) {
                value += statements.at(i)->toString();
                if (i < statements.size() - 1)
                    value += "\n";
            }
            return value;
        }
        case FUNCTION_DECLARATION:
            return "FUNCTION " + name + "\n" + blockStatement->toString() + "\n;";
        case RETURN: {
            string value = "RETURN";
            if (expression != nullptr) {
                value += " " + expression->toString();
            }
            return value;
        }
        case INVALID:
            return "INVALID";
    }*/
   return "abc";
}

//
// StatementFunctionDeclaration
StatementFunctionDeclaration::StatementFunctionDeclaration(string name, shared_ptr<StatementBlock> statementBlock):
    Statement(Statement::Kind::FUNCTION_DECLARATION), name(name), statementBlock(statementBlock) {
}

string StatementFunctionDeclaration::getName() {
    return name;
}

shared_ptr<StatementBlock> StatementFunctionDeclaration::getStatementBlock() {
    return statementBlock;
}

//
// StatementBlock
StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements):
    Statement(Statement::Kind::BLOCK), statements(statements) {
}

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}

//
// StatementReturn
StatementReturn::StatementReturn(shared_ptr<Expression> expression):
    Statement(Statement::Kind::RETURN), expression(expression) {

}

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}

//
// StatementExpression
StatementExpression::StatementExpression(shared_ptr<Expression> expression):
    Statement(Statement::Kind::EXPRESSION), expression(expression) {
}

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}

//
// StatementInvalid
StatementInvalid::StatementInvalid():
    Statement(Statement::Kind::INVALID) {
}
