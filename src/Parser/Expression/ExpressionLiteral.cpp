#include "ExpressionLiteral.h"

#include "Utils.h"
#include "Lexer/Token.h"
#include "Parser/ValueType.h"

shared_ptr<ExpressionLiteral> ExpressionLiteral::expressionLiteralForToken(shared_ptr<Token> token) {
    shared_ptr<ExpressionLiteral> expression = make_shared<ExpressionLiteral>();

    switch (token->getKind()) {
        case TokenKind::BOOL: {
            expression->literalKind = LiteralKind::BOOL;
            expression->valueType = ValueType::LITERAL;

            bool value = token->getLexme().compare("true") == 0;
            expression->boolValue = value;
            expression->uIntValue = 0;
            expression->sIntValue = 0;
            expression->realValue = 0;
            break;
        }
        case TokenKind::INTEGER_DEC: {
            expression->literalKind = LiteralKind::SINT;
            expression->valueType = ValueType::LITERAL;

            string numString = token->getLexme();
            erase(numString, '_');
            int64_t value = stol(numString, nullptr, 10);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->realValue = value;
            break;
        }
        case TokenKind::INTEGER_HEX: {
            expression->literalKind = LiteralKind::UINT;
            expression->valueType = ValueType::LITERAL;

            string numString = token->getLexme();
            erase(numString, '_');
            uint64_t value = stoul(numString, nullptr, 16);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->realValue = value;
            break;
        }
        case TokenKind::INTEGER_BIN: {
            expression->literalKind = LiteralKind::UINT;
            expression->valueType = ValueType::LITERAL;

            string numString = token->getLexme();
            erase(numString, '_');
            numString = numString.substr(2, numString.size()-1);
            uint64_t value = stoul(numString, nullptr, 2);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->realValue = value;
            break;
        }
        case TokenKind::INTEGER_CHAR: {
            expression->literalKind = LiteralKind::UINT;
            expression->valueType = ValueType::LITERAL;

            string charString = token->getLexme();
            optional<uint64_t> value = Utils::charStringToInt(charString);
            if (!value)
                return nullptr;
            expression->boolValue = false;
            expression->uIntValue = *value;
            expression->sIntValue = *value;
            expression->realValue = *value;            
            break;
        }
        case TokenKind::REAL: {
            expression->literalKind = LiteralKind::REAL;
            expression->valueType = ValueType::LITERAL;

            string numString = token->getLexme();
            erase(numString, '_');
            double value = stof(numString);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->realValue = value;
            break;
        }
        default:
            return nullptr;
    }

    return expression;
}

ExpressionLiteral::ExpressionLiteral():
Expression(ExpressionKind::LITERAL, nullptr) { }

LiteralKind ExpressionLiteral::getLiteralKind() {
    return literalKind;
}

bool ExpressionLiteral::getBoolValue() {
    return boolValue;
}

uint64_t ExpressionLiteral::getUIntValue() {
    return uIntValue;
}

int64_t ExpressionLiteral::getSIntValue() {
    return sIntValue;
}

double ExpressionLiteral::getRealValue() {
    return realValue;
}
