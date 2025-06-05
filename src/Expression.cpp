#include "Expression.h"

Expression::Expression(Kind kind):
    kind(kind) {
}

Expression::Kind Expression::getKind() {
    return kind;
}

bool Expression::isValid() {
    return kind != Expression::Kind::INVALID;
}

string Expression::toString() {
    return "EXPRESSION";
}

//
// ExpressionBinary
ExpressionBinary::ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right):
    Expression(Expression::Kind::BINARY), left(left), right(right) {
    switch (token->getKind()) {
        case Token::Kind::EQUAL:
            operation = EQUAL;
            break;
        case Token::Kind::NOT_EQUAL:
            operation = NOT_EQUAL;
            break;
        case Token::Kind::LESS:
            operation = LESS;
            break;
        case Token::Kind::LESS_EQUAL:
            operation = LESS_EQUAL;
            break;
        case Token::Kind::GREATER:
            operation = GREATER;
            break;
        case Token::Kind::GREATER_EQUAL:
            operation = GREATER_EQUAL;
            break;
        case Token::Kind::PLUS:
            operation = ADD;
            break;
        case Token::Kind::MINUS:
            operation = SUB;
            break;
        case Token::Kind::STAR:
            operation = MUL;
            break;
        case Token::Kind::SLASH:
            operation = DIV;
            break;
        case Token::Kind::PERCENT:
            operation = MOD;
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

string ExpressionBinary::toString() {
    switch (operation) {
    case EQUAL:
        return "{= " + left->toString() + " " + right->toString() + "}";
    case NOT_EQUAL:
        return "{!= " + left->toString() + " " + right->toString() + "}";
    case LESS:
        return "{< " + left->toString() + " " + right->toString() + "}";
    case LESS_EQUAL:
        return "{<= " + left->toString() + " " + right->toString() + "}";
    case GREATER:
        return "{> " + left->toString() + " " + right->toString() + "}";
    case GREATER_EQUAL:
        return "{<= " + left->toString() + " " + right->toString() + "}";
    case ADD:
        return "{+ " + left->toString() + " " + right->toString() + "}";
    case SUB:
        return "{- " + left->toString() + " " + right->toString() + "}";
    case MUL:
        return "{* " + left->toString() + " " + right->toString() + "}";
    case DIV:
        return "{/ " + left->toString() + " " + right->toString() + "}";
    case MOD:
        return "{% " + left->toString() + " " + right->toString() + "}";
    }
}

//
// ExpressionLiteral
ExpressionLiteral::ExpressionLiteral(shared_ptr<Token> token):
    Expression(Expression::Kind::LITERAL) {
    integer = stoi(token->getLexme());
}

int64_t ExpressionLiteral::getInteger() {
    return integer;
}

string ExpressionLiteral::toString() {
    return to_string(integer);
}

//
// ExpressionGrouping
ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> expression):
    Expression(Expression::Kind::GROUPING), expression(expression) {
}

shared_ptr<Expression> ExpressionGrouping::getExpression() {
    return expression;
}

string ExpressionGrouping::toString() {
    return "( " + expression->toString() + " )";
}

//
// ExpressionIfElse
ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<StatementBlock> thenBlock, shared_ptr<StatementBlock> elseBlock):
    Expression(Expression::Kind::IF_ELSE), condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {
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

string ExpressionIfElse::toString() {
    string value = "";

    value += "IF(" + condition->toString() + ")\n";
    value += thenBlock->toString();
    if (elseBlock != nullptr) {
        value += "ELSE\n";
        value += elseBlock->toString();
    }
    value += ";";

    return  value;
}

//
// ExpressionInvalid
ExpressionInvalid::ExpressionInvalid(shared_ptr<Token> token):
    Expression(Expression::Kind::INVALID), token(token) {
}

shared_ptr<Token> ExpressionInvalid::getToken() {
    return token;
}

string ExpressionInvalid::toString() {
    return "Invalid token " + token->toString() + " at " + to_string(token->getLine()) + ":" + to_string(token->getColumn()) + "\n";
}