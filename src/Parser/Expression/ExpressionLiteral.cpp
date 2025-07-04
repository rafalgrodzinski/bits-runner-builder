#include "ExpressionLiteral.h"

ExpressionLiteral::ExpressionLiteral():
Expression(ExpressionKind::LITERAL, ValueType::NONE) { }

ExpressionLiteral::ExpressionLiteral(shared_ptr<Token> token):
Expression(ExpressionKind::LITERAL, ValueType::NONE) {
    switch (token->getKind()) {
        case TokenKind::BOOL:
            boolValue = token->getLexme().compare("true") == 0;
            valueType = ValueType::BOOL;
            break;
        case TokenKind::INTEGER_DEC: {
            string numString = token->getLexme();
            erase(numString, '_');
            sint32Value = stoi(numString, nullptr, 10);
            valueType = ValueType::SINT32;
            break;
        }
        case TokenKind::INTEGER_HEX: {
            string numString = token->getLexme();
            erase(numString, '_');
            sint32Value = stoi(numString, nullptr, 16);
            valueType = ValueType::SINT32;
            break;
        }
        case TokenKind::INTEGER_BIN: {
            string numString = token->getLexme();
            erase(numString, '_');
            numString = numString.substr(2, numString.size()-1);
            sint32Value = stoi(numString, nullptr, 2);
            valueType = ValueType::SINT32;
            break;
        }
        case TokenKind::INTEGER_CHAR: {
            string charString = token->getLexme();

            valueType = ValueType::SINT32;
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
            valueType = ValueType::REAL32;
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
