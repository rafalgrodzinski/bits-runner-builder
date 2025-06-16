#include "Expression.h"

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
// ExpressionLiteral
ExpressionLiteral::ExpressionLiteral(shared_ptr<Token> token):
Expression(ExpressionKind::LITERAL, ValueType::NONE) {
    switch (token->getKind()) {
        case TokenKind::BOOL:
            boolValue = token->getLexme().compare("true") == 0;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::INTEGER:
            sint32Value = stoi(token->getLexme());
            valueType = ValueType::SINT32;
            break;
        case TokenKind::REAL:
            real32Value = stof(token->getLexme());
            valueType = ValueType::REAL32;
            break;
        default:
            exit(1);
    }
}

bool ExpressionLiteral::getBoolValue() {
    return boolValue;
}

int32_t ExpressionLiteral::getSInt32Value() {
    return sint32Value;
}

float ExpressionLiteral::getReal32Value() {
    return real32Value;
}

string ExpressionLiteral::toString(int indent) {
    switch (valueType) {
        case ValueType::NONE:
            return "NONE";
        case ValueType::BOOL:
            return boolValue ? "true" : "false";
        case ValueType::SINT32:
            return to_string(sint32Value);
        case ValueType::REAL32:
            return to_string(real32Value);
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
// ExpressionIfElse
ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<StatementBlock> thenBlock, shared_ptr<StatementBlock> elseBlock):
Expression(ExpressionKind::IF_ELSE, ValueType::NONE), condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {
    // Condition must evaluate to bool
    if (condition->getValueType() != ValueType::BOOL)
        exit(1);

    // Return types must match
    shared_ptr<StatementExpression> thenStatementExpression = thenBlock->getStatementExpression();
    shared_ptr<Expression> thenExpression = thenStatementExpression != nullptr ? thenStatementExpression->getExpression() : nullptr;
    shared_ptr<StatementExpression> elseStatementExpression = elseBlock != nullptr ? elseBlock->getStatementExpression() : nullptr;
    shared_ptr<Expression> elseExpression = elseStatementExpression != nullptr ? elseStatementExpression->getExpression() : nullptr;
    if (thenExpression != nullptr && elseExpression != nullptr && thenExpression->getValueType() != elseExpression->getValueType())
        exit(1);

    // get type or default to void
    valueType = thenExpression ? thenExpression->getValueType() : ValueType::NONE;
}

shared_ptr<Expression> ExpressionIfElse::getCondition() {
    return condition;
}

shared_ptr<StatementBlock> ExpressionIfElse::getThenBlock() {
    return thenBlock;
}

shared_ptr<StatementBlock> ExpressionIfElse::getElseBlock() {
    return elseBlock;
}

string ExpressionIfElse::toString(int indent) {
    string value;
    value += "IF(" + condition->toString(0) + "):\n";

    value += thenBlock->toString(indent+1);
    if (elseBlock != nullptr) {
        for (int ind=0; ind<indent; ind++)
            value += "  ";
        value += "ELSE:\n";
        value += elseBlock->toString(indent+1);
    }
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += ";";

    return  value;
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