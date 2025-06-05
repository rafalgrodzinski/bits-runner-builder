#include "Statement.h"

Statement::Statement(Kind kind): kind(kind) {
}

Statement::Kind Statement::getKind() {
    return kind;
}

bool Statement::isValid() {
    return kind != Statement::Kind::INVALID;
}

string Statement::toString() {
   return "STATEMENT";
}

//
// StatementFunctionDeclaration
StatementFunctionDeclaration::StatementFunctionDeclaration(string name, shared_ptr<StatementBlock> statementBlock):
    Statement(Statement::Kind::FUNCTION_DECLARATION), name(name), statementBlock(statementBlock) {
}

string StatementFunctionDeclaration::getName() {
    return name;
}

string StatementFunctionDeclaration::toString() {
    return "FUNCTION(" + name + ")\n" + statementBlock->toString() + "\n;";
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

string StatementBlock::toString() {
    string value;
    for (int i=0; i<statements.size(); i++)
        value += statements.at(i)->toString();
    return value;
}

//
// StatementReturn
StatementReturn::StatementReturn(shared_ptr<Expression> expression):
    Statement(Statement::Kind::RETURN), expression(expression) {

}

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}

string StatementReturn::toString() {
    string value = "RETURN";
    if (expression != nullptr)
        value += "(" + expression->toString() + ")";
    value += "\n";
    return value;
}

//
// StatementExpression
StatementExpression::StatementExpression(shared_ptr<Expression> expression):
    Statement(Statement::Kind::EXPRESSION), expression(expression) {
}

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}

string StatementExpression::toString() {
    return "EXPRESSION(" + expression->toString() + ")";
}

//
// StatementInvalid
StatementInvalid::StatementInvalid(shared_ptr<Token> token):
    Statement(Statement::Kind::INVALID), token(token) {
}

string StatementInvalid::toString() {
    return "Invalid token " + token->toString() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + "\n";
}
