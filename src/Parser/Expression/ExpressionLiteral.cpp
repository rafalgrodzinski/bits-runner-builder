#include "ExpressionLiteral.h"

#include "Lexer/Token.h"
#include "Parser/ValueType.h"

shared_ptr<ExpressionLiteral> ExpressionLiteral::expressionLiteralForToken(shared_ptr<Token> token) {
    shared_ptr<ExpressionLiteral> expression = make_shared<ExpressionLiteral>(token->getLine(), token->getColumn());

    switch (token->getKind()) {
        case TokenKind::BOOL: {
            expression->literalKind = LiteralKind::BOOL;
            expression->valueType = ValueType::BOOL;

            bool value = token->getLexme().compare("true") == 0;
            expression->boolValue = value;
            expression->uIntValue = 0;
            expression->sIntValue = 0;
            expression->floatValue = 0;
            break;
        }
        case TokenKind::INTEGER_DEC: {
            expression->literalKind = LiteralKind::SINT;
            expression->valueType = ValueType::INT;

            string numString = token->getLexme();
            erase(numString, '_');
            int64_t value = stol(numString, nullptr, 10);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->floatValue = value;
            break;
        }
        case TokenKind::INTEGER_HEX: {
            expression->literalKind = LiteralKind::UINT;
            expression->valueType = ValueType::INT;

            string numString = token->getLexme();
            erase(numString, '_');
            uint64_t value = stoul(numString, nullptr, 16);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->floatValue = value;
            break;
        }
        case TokenKind::INTEGER_BIN: {
            expression->literalKind = LiteralKind::UINT;
            expression->valueType = ValueType::INT;

            string numString = token->getLexme();
            erase(numString, '_');
            numString = numString.substr(2, numString.size()-1);
            uint64_t value = stoul(numString, nullptr, 2);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->floatValue = value;
            break;
        }
        case TokenKind::INTEGER_CHAR: {
            expression->literalKind = LiteralKind::UINT;
            expression->valueType = ValueType::INT;

            string charString = token->getLexme();
            optional<uint64_t> value = ExpressionLiteral::charStringToInt(charString);
            if (!value)
                return nullptr;
            expression->boolValue = false;
            expression->uIntValue = *value;
            expression->sIntValue = *value;
            expression->floatValue = *value;            
            break;
        }
        case TokenKind::FLOAT: {
            expression->literalKind = LiteralKind::FLOAT;
            expression->valueType = ValueType::FLOAT;

            string numString = token->getLexme();
            erase(numString, '_');
            double value = stof(numString);
            expression->boolValue = false;
            expression->uIntValue = value;
            expression->sIntValue = value;
            expression->floatValue = value;
            break;
        }
        default:
            return nullptr;
    }

    return expression;
}

shared_ptr<ExpressionLiteral> ExpressionLiteral::expressionLiteralForUInt(uint64_t value, int line, int column) {
    shared_ptr<ExpressionLiteral> expression = make_shared<ExpressionLiteral>(line, column);
    expression->literalKind = LiteralKind::UINT;
    expression->valueType = ValueType::LITERAL;
    expression->boolValue = false;
    expression->uIntValue = value;
    expression->sIntValue = value;
    expression->floatValue = value;
    return expression;
}

ExpressionLiteral::ExpressionLiteral(int line, int column):
Expression(ExpressionKind::LITERAL, nullptr, line, column) { }

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

double ExpressionLiteral::getFloatValue() {
    return floatValue;
}

optional<int> ExpressionLiteral::charStringToInt(string charString) {
    switch (charString.length()) {
        case 1:
            return charString[0];
        case 3:
            return charString[1];
        case 4:
            charString[0] = charString[1];
            charString[1] = charString[2];
        case 2:
            if (charString[0] != '\\')
                return {};
            switch (charString[1]) {
                case 'b':
                    return '\b';
                case 'n':
                    return '\n';
                case 't':
                    return '\t';
                case '\\':
                    return '\\';
                case '\'':
                    return '\'';
                case '\"':
                    return '\"';
                case '0':
                    return '\0';
                default:
                    return {};
            }
    }
    return {};
}