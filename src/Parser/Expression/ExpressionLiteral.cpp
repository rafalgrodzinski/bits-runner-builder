#include "ExpressionLiteral.h"

#include "Utils.h"
#include "Lexer/Token.h"
#include "Parser/ValueType.h"

shared_ptr<ExpressionLiteral> ExpressionLiteral::expressionLiteralForToken(shared_ptr<Token> token) {
    shared_ptr<ExpressionLiteral> expression = make_shared<ExpressionLiteral>();

    switch (token->getKind()) {
        case TokenKind::BOOL:
            expression->boolValue = token->getLexme().compare("true") == 0;
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        case TokenKind::INTEGER_DEC: {
            string numString = token->getLexme();
            erase(numString, '_');
            expression->s32Value = stoi(numString, nullptr, 10);
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_HEX: {
            string numString = token->getLexme();
            erase(numString, '_');
            expression->u32Value = stoul(numString, nullptr, 16);
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_BIN: {
            string numString = token->getLexme();
            erase(numString, '_');
            numString = numString.substr(2, numString.size()-1);
            expression->u32Value = stoul(numString, nullptr, 2);
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_CHAR: {
            string charString = token->getLexme();
            optional<int> charValue = Utils::charStringToInt(charString);
            if (!charValue)
                return nullptr;
            
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            expression->u32Value = *charValue;
            return expression;
        }
        case TokenKind::REAL:
            expression->r32Value = stof(token->getLexme());
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        default:
            return nullptr;
    }

    return expression;
}

ExpressionLiteral::ExpressionLiteral():
Expression(ExpressionKind::LITERAL, nullptr) { }

bool ExpressionLiteral::getBoolValue() {
    return boolValue;
}

uint8_t ExpressionLiteral::getU8Value() {
    return s32Value;
}

uint32_t ExpressionLiteral::getU32Value() {
    return s32Value;
}

int8_t ExpressionLiteral::getS8Value() {
    return s32Value;
}

int32_t ExpressionLiteral::getS32Value() {
    return s32Value;
}

float ExpressionLiteral::getR32Value() {
    return r32Value;
}
