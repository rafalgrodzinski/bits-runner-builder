#include "ValueType.h"

#include "Lexer/Token.h"

shared_ptr<ValueType> ValueType::NONE = make_shared<ValueType>(ValueTypeKind::NONE, nullptr, 0, "");
shared_ptr<ValueType> ValueType::BOOL = make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0, "");
shared_ptr<ValueType> ValueType::U8 = make_shared<ValueType>(ValueTypeKind::U8, nullptr, 0, "");
shared_ptr<ValueType> ValueType::U32 = make_shared<ValueType>(ValueTypeKind::U32, nullptr, 0, "");
shared_ptr<ValueType> ValueType::U64 = make_shared<ValueType>(ValueTypeKind::U64, nullptr, 0, "");
shared_ptr<ValueType> ValueType::S8 = make_shared<ValueType>(ValueTypeKind::S8, nullptr, 0, "");
shared_ptr<ValueType> ValueType::S32 = make_shared<ValueType>(ValueTypeKind::S32, nullptr, 0, "");
shared_ptr<ValueType> ValueType::S64 = make_shared<ValueType>(ValueTypeKind::S64, nullptr, 0, "");
shared_ptr<ValueType> ValueType::R32 = make_shared<ValueType>(ValueTypeKind::R32, nullptr, 0, "");

ValueType::ValueType() { }

ValueType::ValueType(ValueTypeKind kind, shared_ptr<ValueType> subType, int valueArg, string typeName):
kind(kind), subType(subType), valueArg(valueArg), typeName(typeName) { }

shared_ptr<ValueType> ValueType::valueTypeForToken(shared_ptr<Token> token, shared_ptr<ValueType> subType, int valueArg, string typeName) {
    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme.compare("bool") == 0)
                return make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0, "");
            else if (lexme.compare("u8") == 0)
                return make_shared<ValueType>(ValueTypeKind::U8, nullptr, 0, "");
            else if (lexme.compare("u32") == 0)
                return make_shared<ValueType>(ValueTypeKind::U32, nullptr, 0, "");
            else if (lexme.compare("u64") == 0)
                return make_shared<ValueType>(ValueTypeKind::U64, nullptr, 0, "");
            else if (lexme.compare("s8") == 0)
                return make_shared<ValueType>(ValueTypeKind::S8, nullptr, 0, "");
            else if (lexme.compare("s32") == 0)
                return make_shared<ValueType>(ValueTypeKind::S32, nullptr, 0, "");
            else if (lexme.compare("s64") == 0)
                return make_shared<ValueType>(ValueTypeKind::S64, nullptr, 0, "");
            else if (lexme.compare("r32") == 0)
                return make_shared<ValueType>(ValueTypeKind::R32, nullptr, 0, "");
            else if (lexme.compare("data") == 0)
                return make_shared<ValueType>(ValueTypeKind::DATA, subType, valueArg, "");
            else if (lexme.compare("blob") == 0)
                return make_shared<ValueType>(ValueTypeKind::BLOB, nullptr, 0, typeName);
            else if (lexme.compare("ptr") == 0)
                return make_shared<ValueType>(ValueTypeKind::PTR, subType, 0, "");
            else
                return nullptr;
        }
        case TokenKind::BOOL:
            return make_shared<ValueType>(ValueTypeKind::BOOL, nullptr, 0, "");
        case TokenKind::INTEGER_DEC:
            return make_shared<ValueType>(ValueTypeKind::S32, nullptr, 0, "");
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            return make_shared<ValueType>(ValueTypeKind::U32, nullptr, 0, "");
        case TokenKind::REAL:
            return make_shared<ValueType>(ValueTypeKind::R32, nullptr, 0, "");
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

string ValueType::getTypeName() {
    return typeName;
}