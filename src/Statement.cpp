#include "Statement.h"

//
// Statement
Statement::Statement(StatementKind kind): kind(kind) {
}

StatementKind Statement::getKind() {
    return kind;
}

bool Statement::isValid() {
    return kind != StatementKind::INVALID;
}

string Statement::toString(int indent) {
   return "STATEMENT";
}

//
// StatementFunctionDeclaration
StatementFunctionDeclaration::StatementFunctionDeclaration(string name, ValueType returnValueType, shared_ptr<StatementBlock> statementBlock):
Statement(StatementKind::FUNCTION_DECLARATION), name(name), returnValueType(returnValueType), statementBlock(statementBlock) {
}

string StatementFunctionDeclaration::getName() {
    return name;
}

ValueType StatementFunctionDeclaration::getReturnValueType() {
    return returnValueType;
}

shared_ptr<StatementBlock> StatementFunctionDeclaration::getStatementBlock() {
    return statementBlock;
}

string StatementFunctionDeclaration::toString(int indent) {
    string value = "";
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "FUNCTION(" + name + "):\n";
    value += statementBlock->toString(indent+1);
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += ";";
    return value;
}

//
// StatementVarDeclaration
StatementVarDeclaration::StatementVarDeclaration(string name, ValueType valueType, shared_ptr<Expression> expression):
Statement(StatementKind::VAR_DECLARATION), name(name), valueType(valueType), expression(expression) {
}

string StatementVarDeclaration::getName() {
    return name;
}

ValueType StatementVarDeclaration::getValueType() {
    return valueType;
}

shared_ptr<Expression> StatementVarDeclaration::getExpression() {
    return expression;
}

string StatementVarDeclaration::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += name + "(";
    switch (valueType) {
        case ValueType::VOID:
            value += "VOID";
            break;
        case ValueType::BOOL:
            value += "BOOL";
            break;
        case ValueType::SINT32:
            value += "SINT32";
            break;
        case ValueType::REAL32:
            value += "REAL32";
            break;
    }
    value += "):\n";
    for (int ind=0; ind<indent+1; ind++)
        value += "  ";
    value += expression->toString(indent+1);
    value += "\n";
    return value;
}

//
// StatementBlock
StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements):
Statement(StatementKind::BLOCK), statements(statements) {
    if (statements.back()->getKind() == StatementKind::EXPRESSION) {
        statementExpression = dynamic_pointer_cast<StatementExpression>(statements.back());
        this->statements.pop_back();
    }
}

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}

shared_ptr<StatementExpression> StatementBlock::getStatementExpression() {
    return statementExpression;
}

string StatementBlock::toString(int indent) {
    string value;
    for (int i=0; i<statements.size(); i++) {
        //for (int ind=0; ind<indent; ind++)
        //    value += "  ";
        value += statements.at(i)->toString(indent);
    }
    if (statementExpression != nullptr) {
        for (int ind=0; ind<indent; ind++)
            value += "  ";
        value += "WRAP_UP:\n";
        value += statementExpression->toString(indent);
    }
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "#\n";
    return value;
}

//
// StatementReturn
StatementReturn::StatementReturn(shared_ptr<Expression> expression):
Statement(StatementKind::RETURN), expression(expression) {
}

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}

string StatementReturn::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "RETURN:\n";
    for (int ind=0; ind<indent+1; ind++)
        value += "  ";
    value += expression->toString(indent+1);
    value += "\n";
    return value;
}

//
// StatementExpression
StatementExpression::StatementExpression(shared_ptr<Expression> expression):
Statement(StatementKind::EXPRESSION), expression(expression) {
}

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}

string StatementExpression::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += expression->toString(indent);
    value += "\n";
    return value;
}

//
// StatementInvalid
StatementInvalid::StatementInvalid(shared_ptr<Token> token, string message):
Statement(StatementKind::INVALID), token(token), message(message) {
}

string StatementInvalid::toString(int indent) {
    return "Error for token " + token->getLexme() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + ": " + message + "\n";
}

string StatementInvalid::getMessage() {
    return message;
}
