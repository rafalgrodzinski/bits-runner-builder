#include "ExpressionLiteral.h"

#include "Utils.h"
#include "Lexer/Token.h"
#include "Parser/ValueType.h"

shared_ptr<ExpressionLiteral> ExpressionLiteral::expressionLiteralForToken(shared_ptr<Token> token) {
    switch (token->getKind()) {
        case TokenKind::INTEGER_CHAR: {
            string charString = token->getLexme();
            optional<int> charValue = Utils::charStringToInt(charString);
            if (!charValue)
                return nullptr;
            shared_ptr<ExpressionLiteral> expression = make_shared<ExpressionLiteral>();
            expression->valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            expression->sint32Value = *charValue;
            return expression;
        }
        default:
            return nullptr;
    }
}

ExpressionLiteral::ExpressionLiteral():
Expression(ExpressionKind::LITERAL, nullptr) { }

ExpressionLiteral::ExpressionLiteral(shared_ptr<Token> token):
Expression(ExpressionKind::LITERAL, nullptr) {
    switch (token->getKind()) {
        case TokenKind::BOOL:
            boolValue = token->getLexme().compare("true") == 0;
            valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        case TokenKind::INTEGER_DEC: {
            string numString = token->getLexme();
            erase(numString, '_');
            sint32Value = stoi(numString, nullptr, 10);
            valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_HEX: {
            string numString = token->getLexme();
            erase(numString, '_');
            sint32Value = stoi(numString, nullptr, 16);
            valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_BIN: {
            string numString = token->getLexme();
            erase(numString, '_');
            numString = numString.substr(2, numString.size()-1);
            sint32Value = stoi(numString, nullptr, 2);
            valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            break;
        }
        case TokenKind::INTEGER_CHAR: {
            string charString = token->getLexme();

            valueType = ValueType::valueTypeForToken(token, nullptr, 0);
            if (charString.length() == 3) {
                sint32Value = charString[1];
            } else if (charString.length() == 4 && charString[1] == '\\') {
                switch (charString[2]) {
                    case 'b':
                        sint32Value = '\b';
                        break;
                    case 'n':
                        sint32Value = '\n';
                        break;
                    case 't':
                        sint32Value = '\t';
                        break;
                    case '\\':
                        sint32Value = '\\';
                        break;
                    case '\'':
                        sint32Value = '\'';
                        break;
                    case '\"':
                        sint32Value = '\"';
                        break;
                }
            }
            break;
        }
        case TokenKind::REAL:
            real32Value = stof(token->getLexme());
            valueType = ValueType::valueTypeForToken(token, nullptr, 0);
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
