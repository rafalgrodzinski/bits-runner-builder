#include "ValueType.h"

#include "Lexer/Token.h"

shared_ptr<ValueType> ValueType::NONE = make_shared<ValueType>(ValueTypeKind::NONE, nullptr, 0);
shared_ptr<ValueType> ValueType::BOOL = make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0);
shared_ptr<ValueType> ValueType::U8 = make_shared<ValueType>(ValueTypeKind::U8, nullptr, 0);
shared_ptr<ValueType> ValueType::U32 = make_shared<ValueType>(ValueTypeKind::U32, nullptr, 0);
shared_ptr<ValueType> ValueType::S8 = make_shared<ValueType>(ValueTypeKind::S8, nullptr, 0);
shared_ptr<ValueType> ValueType::S32 = make_shared<ValueType>(ValueTypeKind::S32, nullptr, 0);
shared_ptr<ValueType> ValueType::R32 = make_shared<ValueType>(ValueTypeKind::R32, nullptr, 0);

ValueType::ValueType(ValueTypeKind kind, shared_ptr<ValueType> subType, int valueArg):
kind(kind), subType(subType), valueArg(valueArg) { }

shared_ptr<ValueType> ValueType::valueTypeForToken(shared_ptr<Token> token, shared_ptr<ValueType> subType, int valueArg) {
    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme.compare("bool") == 0)
                return make_shared<ValueType>(ValueTypeKind::BOOL, subType, valueArg);
            else if (lexme.compare("u8") == 0)
                return make_shared<ValueType>(ValueTypeKind::U8, subType, valueArg);
            else if (lexme.compare("u32") == 0)
                return make_shared<ValueType>(ValueTypeKind::U32, subType, valueArg);
            else if (lexme.compare("s8") == 0)
                return make_shared<ValueType>(ValueTypeKind::S8, subType, valueArg);
            else if (lexme.compare("s32") == 0)
                return make_shared<ValueType>(ValueTypeKind::S32, subType, valueArg);
            else if (lexme.compare("r32") == 0)
                return make_shared<ValueType>(ValueTypeKind::R32, subType, valueArg);
            else if (lexme.compare("data") == 0)
                return make_shared<ValueType>(ValueTypeKind::DATA, subType, valueArg);
            else
                return nullptr;
        }
        case TokenKind::BOOL:
            return make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0);
        case TokenKind::INTEGER_DEC:
            return make_shared<ValueType>(ValueTypeKind::S32, nullptr, 0);
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            return make_shared<ValueType>(ValueTypeKind::U32, nullptr, 0);
        case TokenKind::REAL:
            return make_shared<ValueType>(ValueTypeKind::R32, nullptr, 0);
        default:
            return nullptr;
    }
}

ValueTypeKind ValueType::getKind() {
    return kind;
}

shared_ptr<ValueType> ValueType::getSubType() {
    return subType;
}

int ValueType::getValueArg() {
    return valueArg;
}