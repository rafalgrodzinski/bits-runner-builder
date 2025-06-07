#include "Expression.h"

Expression::Expression(Kind kind, ValueType valueType):
    kind(kind), valueType(valueType) {
}

Expression::Kind Expression::getKind() {
    return kind;
}

Expression::ValueType Expression::getValueType() {
    return valueType;
}

bool Expression::isValid() {
    return kind != Expression::Kind::INVALID;
}

string Expression::toString(int indent) {
    return "EXPRESSION";
}

//
// ExpressionBinary
ExpressionBinary::ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right):
    Expression(Expression::Kind::BINARY, Expression::ValueType::VOID), left(left), right(right) {
    // Types must match
    if (left->getValueType() != right->getValueType())
        exit(1);

    // Booleans can only do = or !=
    if (valueType == Expression::ValueType::BOOL && (token->getKind() != Token::Kind::EQUAL || token->getKind() != Token::Kind::NOT_EQUAL))
        exit(1);

    switch (token->getKind()) {
        case Token::Kind::EQUAL:
            operation = EQUAL;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::NOT_EQUAL:
            operation = NOT_EQUAL;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::LESS:
            operation = LESS;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::LESS_EQUAL:
            operation = LESS_EQUAL;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::GREATER:
            operation = GREATER;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::GREATER_EQUAL:
            operation = GREATER_EQUAL;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::PLUS:
            operation = ADD;
            valueType = left->getValueType();
            break;
        case Token::Kind::MINUS:
            operation = SUB;
            valueType = left->getValueType();
            break;
        case Token::Kind::STAR:
            operation = MUL;
            valueType = left->getValueType();
            break;
        case Token::Kind::SLASH:
            operation = DIV;
            valueType = left->getValueType();
            break;
        case Token::Kind::PERCENT:
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
    Expression(Expression::Kind::LITERAL, Expression::ValueType::VOID) {
    switch (token->getKind()) {
        case Token::Kind::BOOL:
            boolValue = token->getLexme().compare("true") == 0;
            valueType = Expression::ValueType::BOOL;
            break;
        case Token::Kind::INTEGER:
            sint32Value = stoi(token->getLexme());
            valueType = Expression::ValueType::SINT32;
            break;
        case Token::Kind::REAL:
            real32Value = stof(token->getLexme());
            valueType = Expression::ValueType::REAL32;
            break;
        default:
            exit(1);
    }
}

bool ExpressionLiteral::getBoolValue() {
    return boolValue;
}

int32_t ExpressionLiteral::getSint32Value() {
    return sint32Value;
}

float ExpressionLiteral::getReal32Value() {
    return real32Value;
}

string ExpressionLiteral::toString(int indent) {
    switch (valueType) {
        case Expression::ValueType::VOID:
            return "VOID";
        case Expression::ValueType::BOOL:
            return to_string(boolValue);
        case Expression::ValueType::SINT32:
            return to_string(sint32Value);
        case Expression::ValueType::REAL32:
            return to_string(real32Value);
    }
}

//
// ExpressionGrouping
ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> expression):
    Expression(Expression::Kind::GROUPING, expression->getValueType()), expression(expression) {
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
    Expression(Expression::Kind::IF_ELSE, Expression::ValueType::VOID), condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {
    // Condition must evaluate to bool
    if (condition->getValueType() != Expression::ValueType::BOOL)
        exit(1);

    // Return types must match
    shared_ptr<StatementExpression> thenStatementExpression = thenBlock->getStatementExpression();
    shared_ptr<Expression> thenExpression = thenStatementExpression != nullptr ? thenStatementExpression->getExpression() : nullptr;
    shared_ptr<StatementExpression> elseStatementExpression = elseBlock != nullptr ? elseBlock->getStatementExpression() : nullptr;
    shared_ptr<Expression> elseExpression = elseStatementExpression != nullptr ? elseStatementExpression->getExpression() : nullptr;
    if (thenExpression != nullptr && elseExpression != nullptr && thenExpression->getValueType() != elseExpression->getValueType())
        exit(1);

    // get type or default to void
    valueType = thenExpression ? thenExpression->getValueType() : Expression::ValueType::VOID;
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
// ExpressionInvalid
ExpressionInvalid::ExpressionInvalid(shared_ptr<Token> token):
    Expression(Expression::Kind::INVALID, Expression::ValueType::VOID), token(token) {
}

shared_ptr<Token> ExpressionInvalid::getToken() {
    return token;
}

string ExpressionInvalid::toString(int indent) {
    return "Invalid token " + token->toString() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + "\n";
}