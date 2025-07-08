#include "ValueType.h"

#include "Lexer/Token.h"

shared_ptr<ValueType> ValueType::NONE = make_shared<ValueType>(ValueTypeKind::NONE, nullptr, 0);
shared_ptr<ValueType> ValueType::BOOL = make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0);
shared_ptr<ValueType> ValueType::SINT32 = make_shared<ValueType>(ValueTypeKind::SINT32, nullptr, 0);
shared_ptr<ValueType> ValueType::REAL32 = make_shared<ValueType>(ValueTypeKind::REAL32, nullptr, 0);

ValueType::ValueType(ValueTypeKind kind, shared_ptr<ValueType> subType, int valueArg):
kind(kind), subType(subType), valueArg(valueArg) { }

shared_ptr<ValueType> ValueType::valueTypeForToken(shared_ptr<Token> token, shared_ptr<ValueType> subType, int valueArg) {
    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme.compare("bool") == 0)
                return make_shared<ValueType>(ValueTypeKind::BOOL, subType, valueArg);
            else if (lexme.compare("sint32") == 0)
                return make_shared<ValueType>(ValueTypeKind::SINT32, subType, valueArg);
            else if (lexme.compare("real32") == 0)
                return make_shared<ValueType>(ValueTypeKind::REAL32, subType, valueArg);
            else if (lexme.compare("data") == 0)
                return make_shared<ValueType>(ValueTypeKind::DATA, subType, valueArg);
            else
                return nullptr;
        }
        case TokenKind::BOOL:
            return make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0);
        case TokenKind::INTEGER_DEC:
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            return make_shared<ValueType>(ValueTypeKind::SINT32, nullptr, 0);
        case TokenKind::REAL:
            return make_shared<ValueType>(ValueTypeKind::REAL32, nullptr, 0);
        default:
            return nullptr;
    }
}

ValueTypeKind ValueType::getKind() {
    return kind;
}

shared_ptr<ValueType> getSubType() {}