#include "ValueTypeSimple.h"
#include "Lexer/Token.h"

shared_ptr<ValueType> ValueTypeSimple::NONE = make_shared<ValueTypeSimple>(ValueTypeKind::NONE, nullptr);
shared_ptr<ValueType> ValueTypeSimple::BOOL = make_shared<ValueTypeSimple>(ValueTypeKind::BOOL, nullptr);
shared_ptr<ValueType> ValueTypeSimple::UINT = make_shared<ValueTypeSimple>(ValueTypeKind::UINT, nullptr);
shared_ptr<ValueType> ValueTypeSimple::U8 = make_shared<ValueTypeSimple>(ValueTypeKind::U8, nullptr);
shared_ptr<ValueType> ValueTypeSimple::U16 = make_shared<ValueTypeSimple>(ValueTypeKind::U16, nullptr);
shared_ptr<ValueType> ValueTypeSimple::U32 = make_shared<ValueTypeSimple>(ValueTypeKind::U32, nullptr);
shared_ptr<ValueType> ValueTypeSimple::U64 = make_shared<ValueTypeSimple>(ValueTypeKind::U64, nullptr);
shared_ptr<ValueType> ValueTypeSimple::SINT = make_shared<ValueTypeSimple>(ValueTypeKind::SINT, nullptr);
shared_ptr<ValueType> ValueTypeSimple::S8 = make_shared<ValueTypeSimple>(ValueTypeKind::S8, nullptr);
shared_ptr<ValueType> ValueTypeSimple::S16 = make_shared<ValueTypeSimple>(ValueTypeKind::S16, nullptr);
shared_ptr<ValueType> ValueTypeSimple::S32 = make_shared<ValueTypeSimple>(ValueTypeKind::S32, nullptr);
shared_ptr<ValueType> ValueTypeSimple::S64 = make_shared<ValueTypeSimple>(ValueTypeKind::S64, nullptr);
shared_ptr<ValueType> ValueTypeSimple::FLOAT = make_shared<ValueTypeSimple>(ValueTypeKind::FLOAT, nullptr);
shared_ptr<ValueType> ValueTypeSimple::F32 = make_shared<ValueTypeSimple>(ValueTypeKind::F32, nullptr);
shared_ptr<ValueType> ValueTypeSimple::F64 = make_shared<ValueTypeSimple>(ValueTypeKind::F64, nullptr);
shared_ptr<ValueType> ValueTypeSimple::A = make_shared<ValueTypeSimple>(ValueTypeKind::A, nullptr);

shared_ptr<ValueTypeSimple> ValueTypeSimple::simpleForToken(shared_ptr<Token> token, shared_ptr<Location> location) {
    shared_ptr<ValueTypeSimple> valueTypeSimple = nullptr;

    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme == "bool") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::BOOL, location);
            } else if (lexme == "u8") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U8, location);
            } else if (lexme == "u16") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U16, location);
            } else if (lexme == "u32") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U32, location);
            } else if (lexme == "u64") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::U64, location);
            } else if (lexme == "s8") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S8, location);
            } else if (lexme == "s16") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S16, location);
            } else if (lexme == "s32") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S32, location);
            } else if (lexme == "s64") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::S64, location);
            } else if (lexme == "f32") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::F32, location);
            } else if (lexme == "f64") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::F64, location);
            } else if (lexme == "a") {
                valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::A, location);
            } else {
                return nullptr;
            }
            break;
        }
        case TokenKind::BOOL:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::BOOL, location);
            break;
        case TokenKind::INTEGER_DEC:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::SINT, location);
            break;
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::UINT, location);
            break;
        case TokenKind::FLOAT:
            valueTypeSimple = make_shared<ValueTypeSimple>(ValueTypeKind::FLOAT, location);
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

ValueTypeSimple::ValueTypeSimple(ValueTypeKind kind, shared_ptr<Location> location):
ValueType(kind, location) { }