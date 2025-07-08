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
            expression->sint32Value = stoi(numString, nullptr, 10);
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_HEX: {
            string numString = token->getLexme();
            erase(numString, '_');
            expression->sint32Value = stoi(numString, nullptr, 16);
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_BIN: {
            string numString = token->getLexme();
            erase(numString, '_');
            numString = numString.substr(2, numString.size()-1);
            expression->sint32Value = stoi(numString, nullptr, 2);
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_CHAR: {
            string charString = token->getLexme();
            optional<int> charValue = Utils::charStringToInt(charString);
            if (!charValue)
                return nullptr;
            
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            expression->sint32Value = *charValue;
            return expression;
        }
        case TokenKind::REAL:
            expression->real32Value = stof(token->getLexme());
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

int32_t ExpressionLiteral::getSint32Value() {
    return sint32Value;
}

float ExpressionLiteral::getReal32Value() {
    return real32Value;
}
