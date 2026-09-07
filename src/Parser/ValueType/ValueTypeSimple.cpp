#include "ValueTypeSimple.h"
#include "Lexer/Token.h"

shared_ptr<ValueType> ValueTypeSimple::NONE = make_shared<ValueTypeSimple>(ValueTypeKind::NONE);
shared_ptr<ValueType> ValueTypeSimple::BOOL = make_shared<ValueTypeSimple>(ValueTypeKind::BOOL);
shared_ptr<ValueType> ValueTypeSimple::UINT = make_shared<ValueTypeSimple>(ValueTypeKind::UINT);
shared_ptr<ValueType> ValueTypeSimple::U8 = make_shared<ValueTypeSimple>(ValueTypeKind::U8);
shared_ptr<ValueType> ValueTypeSimple::U16 = make_shared<ValueTypeSimple>(ValueTypeKind::U16);
shared_ptr<ValueType> ValueTypeSimple::U32 = make_shared<ValueTypeSimple>(ValueTypeKind::U32);
shared_ptr<ValueType> ValueTypeSimple::U64 = make_shared<ValueTypeSimple>(ValueTypeKind::U64);
shared_ptr<ValueType> ValueTypeSimple::SINT = make_shared<ValueTypeSimple>(ValueTypeKind::SINT);
shared_ptr<ValueType> ValueTypeSimple::S8 = make_shared<ValueTypeSimple>(ValueTypeKind::S8);
shared_ptr<ValueType> ValueTypeSimple::S16 = make_shared<ValueTypeSimple>(ValueTypeKind::S16);
shared_ptr<ValueType> ValueTypeSimple::S32 = make_shared<ValueTypeSimple>(ValueTypeKind::S32);
shared_ptr<ValueType> ValueTypeSimple::S64 = make_shared<ValueTypeSimple>(ValueTypeKind::S64);
shared_ptr<ValueType> ValueTypeSimple::FLOAT = make_shared<ValueTypeSimple>(ValueTypeKind::FLOAT);
shared_ptr<ValueType> ValueTypeSimple::F32 = make_shared<ValueTypeSimple>(ValueTypeKind::F32);
shared_ptr<ValueType> ValueTypeSimple::F64 = make_shared<ValueTypeSimple>(ValueTypeKind::F64);
shared_ptr<ValueType> ValueTypeSimple::A = make_shared<ValueTypeSimple>(ValueTypeKind::A);

shared_ptr<ValueTypeSimple> ValueTypeSimple::simpleForToken(shared_ptr<Token> token) {
    shared_ptr<ValueTypeSimple> valueTypeSimple = nullptr;

    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme == "bool") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::BOOL);
            } else if (lexme == "u8") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U8);
            } else if (lexme == "u16") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U16);
            } else if (lexme == "u32") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U32);
            } else if (lexme == "u64") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U64);
            } else if (lexme == "s8") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S8);
            } else if (lexme == "s16") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S16);
            } else if (lexme == "s32") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S32);
            } else if (lexme == "s64") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S64);
            } else if (lexme == "f32") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::F32);
            } else if (lexme == "f64") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::F64);
            } else if (lexme == "a") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::A);
            } else {
                return nullptr;
            }
            break;
        }
        case TokenKind::BOOL:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::BOOL);
            break;
        case TokenKind::INTEGER_DEC:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::SINT);
            break;
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::UINT);
            break;
        case TokenKind::FLOAT:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::FLOAT);
            break;
        default:
            return nullptr;
    }

    return valueTypeSimple;
}

bool ValueTypeSimple::isEqual(shared_ptr<ValueType> other) const {
    shared_ptr<ValueTypeSimple> otherValueTypeSimple = dynamic_pointer_cast<ValueTypeSimple>(other);
    if (otherValueTypeSimple == nullptr)
        return false;

    return getKind() == otherValueTypeSimple->getKind();
}

shared_ptr<ValueType> ValueTypeSimple::clone() const {
    return make_shared<ValueTypeSimple>(*this);
}

ValueTypeSimple::ValueTypeSimple(ValueTypeKind kind):
ValueType(kind) { }