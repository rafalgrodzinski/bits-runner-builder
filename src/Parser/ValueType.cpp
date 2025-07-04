#include "ValueType.h"

#include "Lexer/Token.h"

shared_ptr<ValueType> ValueType::NONE = make_shared<ValueType>(ValueTypeKind::NONE);
shared_ptr<ValueType> ValueType::BOOL = make_shared<ValueType>(ValueTypeKind::BOOL);
shared_ptr<ValueType> ValueType::SINT32 = make_shared<ValueType>(ValueTypeKind::SINT32);
shared_ptr<ValueType> ValueType::REAL32 = make_shared<ValueType>(ValueTypeKind::REAL32);

ValueType::ValueType(ValueTypeKind kind):
kind(kind) { }

shared_ptr<ValueType> ValueType::valueTypeForToken(shared_ptr<Token> token) {
    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme.compare("bool") == 0)
                return make_shared<ValueType>(ValueTypeKind::BOOL);
            else if (lexme.compare("sint32") == 0)
                return make_shared<ValueType>(ValueTypeKind::SINT32);
            else if (lexme.compare("real32") == 0)
                return make_shared<ValueType>(ValueTypeKind::REAL32);
            else
                return nullptr;
        }
        case TokenKind::BOOL:
            return make_shared<ValueType>(ValueTypeKind::BOOL);
        case TokenKind::INTEGER_DEC:
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            return make_shared<ValueType>(ValueTypeKind::SINT32);
        case TokenKind::REAL:
            return make_shared<ValueType>(ValueTypeKind::REAL32);
        default:
            return nullptr;
    }
}

ValueTypeKind ValueType::getKind() {
    return kind;
}