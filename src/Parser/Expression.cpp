#include "Expression.h"

#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementBlock.h"

Expression::Expression(ExpressionKind kind, ValueType valueType):
    kind(kind), valueType(valueType) {
}

ExpressionKind Expression::getKind() {
    return kind;
}

ValueType Expression::getValueType() {
    return valueType;
}

bool Expression::isValid() {
    return kind != ExpressionKind::INVALID;
}

string Expression::toString(int indent) {
    return "EXPRESSION";
}

//
// ExpressionBinary
ExpressionBinary::ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right):
Expression(ExpressionKind::BINARY, ValueType::NONE), left(left), right(right) {
    // Types must match
    if (left->getValueType() != right->getValueType())
        exit(1);

    // Booleans can only do = or !=
    if (valueType == ValueType::BOOL && (token->getKind() != TokenKind::EQUAL || token->getKind() != TokenKind::NOT_EQUAL))
        exit(1);

    switch (token->getKind()) {
        case TokenKind::EQUAL:
            operation = EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::NOT_EQUAL:
            operation = NOT_EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::LESS:
            operation = LESS;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::LESS_EQUAL:
            operation = LESS_EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::GREATER:
            operation = GREATER;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::GREATER_EQUAL:
            operation = GREATER_EQUAL;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::PLUS:
            operation = ADD;
            valueType = left->getValueType();
            break;
        case TokenKind::MINUS:
            operation = SUB;
            valueType = left->getValueType();
            break;
        case TokenKind::STAR:
            operation = MUL;
            valueType = left->getValueType();
            break;
        case TokenKind::SLASH:
            operation = DIV;
            valueType = left->getValueType();
            break;
        case TokenKind::PERCENT:
            operation = MOD;
            valueType = left->getValueType();
            break;
        default:
            exit(1);
    }
}

ExpressionBinary::Operation ExpressionBinary::getOperation() {
    return operation;
}

shared_ptr<Expression> ExpressionBinary::getLeft() {
    return left;
}

shared_ptr<Expression> ExpressionBinary::getRight() {
    return right;
}

string ExpressionBinary::toString(int indent) {
    switch (operation) {
    case EQUAL:
        return "{= " + left->toString(0) + " " + right->toString(0) + "}";
    case NOT_EQUAL:
        return "{!= " + left->toString(0) + " " + right->toString(0) + "}";
    case LESS:
        return "{< " + left->toString(0) + " " + right->toString(0) + "}";
    case LESS_EQUAL:
        return "{<= " + left->toString(0) + " " + right->toString(0) + "}";
    case GREATER:
        return "{> " + left->toString(0) + " " + right->toString(0) + "}";
    case GREATER_EQUAL:
        return "{<= " + left->toString(0) + " " + right->toString(0) + "}";
    case ADD:
        return "{+ " + left->toString(0) + " " + right->toString(0) + "}";
    case SUB:
        return "{- " + left->toString(0) + " " + right->toString(0) + "}";
    case MUL:
        return "{* " + left->toString(0) + " " + right->toString(0) + "}";
    case DIV:
        return "{/ " + left->toString(0) + " " + right->toString(0) + "}";
    case MOD:
        return "{% " + left->toString(0) + " " + right->toString(0) + "}";
    }
}

//
// ExpressionGrouping
ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> expression):
Expression(ExpressionKind::GROUPING, expression->getValueType()), expression(expression) {
}

shared_ptr<Expression> ExpressionGrouping::getExpression() {
    return expression;
}

string ExpressionGrouping::toString(int indent) {
    return "( " + expression->toString(0) + " )";
}

//
// ExpressionVar
ExpressionVar::ExpressionVar(string name):
Expression(ExpressionKind::VAR, ValueType::NONE), name(name) {
}

string ExpressionVar::getName() {
    return name;
}

string ExpressionVar::toString(int indent) {
    return "VAR(" + name + ")";
}

//
// Expression Call
ExpressionCall::ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions):
Expression(ExpressionKind::CALL, ValueType::NONE), name(name), argumentExpressions(argumentExpressions) {
}

string ExpressionCall::getName() {
    return name;
}

vector<shared_ptr<Expression>> ExpressionCall::getArgumentExpressions() {
    return argumentExpressions;
}

string ExpressionCall::toString(int indent) {
    string value;

    value += "CALL(" + name + "):";
    for (shared_ptr<Expression> &argumentExpression : argumentExpressions) {
        value += "\n";
        for (int ind=0; ind<indent+1; ind++)
            value += "  ";
        value += argumentExpression->toString(indent+1) + ",";
    }

    return value;
}

//
// ExpressionInvalid
ExpressionInvalid::ExpressionInvalid(shared_ptr<Token> token):
Expression(ExpressionKind::INVALID, ValueType::NONE), token(token) {
}

shared_ptr<Token> ExpressionInvalid::getToken() {
    return token;
}

string ExpressionInvalid::toString(int indent) {
    return "Invalid token " + token->toString() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + "\n";
}