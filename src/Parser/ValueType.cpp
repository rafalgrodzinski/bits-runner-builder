#include "ValueType.h"

#include "Lexer/Token.h"
#include "Parser/Expression/ExpressionLiteral.h"

shared_ptr<ValueType> ValueType::NONE = make_shared<ValueType>(ValueTypeKind::NONE);
shared_ptr<ValueType> ValueType::BOOL = make_shared<ValueType>(ValueTypeKind::BOOL);
shared_ptr<ValueType> ValueType::UINT = make_shared<ValueType>(ValueTypeKind::UINT);
shared_ptr<ValueType> ValueType::U8 = make_shared<ValueType>(ValueTypeKind::U8);
shared_ptr<ValueType> ValueType::U16 = make_shared<ValueType>(ValueTypeKind::U16);
shared_ptr<ValueType> ValueType::U32 = make_shared<ValueType>(ValueTypeKind::U32);
shared_ptr<ValueType> ValueType::U64 = make_shared<ValueType>(ValueTypeKind::U64);
shared_ptr<ValueType> ValueType::SINT = make_shared<ValueType>(ValueTypeKind::SINT);
shared_ptr<ValueType> ValueType::S8 = make_shared<ValueType>(ValueTypeKind::S8);
shared_ptr<ValueType> ValueType::S16 = make_shared<ValueType>(ValueTypeKind::S16);
shared_ptr<ValueType> ValueType::S32 = make_shared<ValueType>(ValueTypeKind::S32);
shared_ptr<ValueType> ValueType::S64 = make_shared<ValueType>(ValueTypeKind::S64);
shared_ptr<ValueType> ValueType::FLOAT = make_shared<ValueType>(ValueTypeKind::FLOAT);
shared_ptr<ValueType> ValueType::F32 = make_shared<ValueType>(ValueTypeKind::F32);
shared_ptr<ValueType> ValueType::F64 = make_shared<ValueType>(ValueTypeKind::F64);
shared_ptr<ValueType> ValueType::A = make_shared<ValueType>(ValueTypeKind::A);

shared_ptr<ValueType> ValueType::simpleForToken(shared_ptr<Token> token) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();

    switch (token->getKind()) {
        case TokenKind::TYPE: {
            string lexme = token->getLexme();
            if (lexme.compare("bool") == 0) {
                valueType->kind = ValueTypeKind::BOOL;
            } else if (lexme.compare("u8") == 0) {
                valueType->kind = ValueTypeKind::U8;
            } else if (lexme.compare("u16") == 0) {
                valueType->kind = ValueTypeKind::U16;
            } else if (lexme.compare("u32") == 0) {
                valueType->kind = ValueTypeKind::U32;
            } else if (lexme.compare("u64") == 0) {
                valueType->kind = ValueTypeKind::U64;
            } else if (lexme.compare("s8") == 0) {
                valueType->kind = ValueTypeKind::S8;
            } else if (lexme.compare("s16") == 0) {
                valueType->kind = ValueTypeKind::S16;
            } else if (lexme.compare("s32") == 0) {
                valueType->kind = ValueTypeKind::S32;
            } else if (lexme.compare("s64") == 0) {
                valueType->kind = ValueTypeKind::S64;
            } else if (lexme.compare("f32") == 0) {
                valueType->kind = ValueTypeKind::F32;
            } else if (lexme.compare("f64") == 0) {
                valueType->kind = ValueTypeKind::F64;
            } else if (lexme.compare("a") == 0) {
                valueType->kind = ValueTypeKind::A;
            } else {
                return nullptr;
            }
            break;
        }
        case TokenKind::BOOL:
            valueType->kind = ValueTypeKind::BOOL;
            break;
        case TokenKind::INTEGER_DEC:
            valueType->kind = ValueTypeKind::SINT;
            break;
        case TokenKind::INTEGER_HEX:
        case TokenKind::INTEGER_BIN:
        case TokenKind::INTEGER_CHAR:
            valueType->kind = ValueTypeKind::UINT;
            break;
        case TokenKind::FLOAT:
            valueType->kind = ValueTypeKind::FLOAT;
            break;
        default:
            return nullptr;
    }

    return valueType;
}

shared_ptr<ValueType> ValueType::data(shared_ptr<ValueType> subType, shared_ptr<Expression> countExpression) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::DATA;
    valueType->subType = subType;
    valueType->countExpression = countExpression;
    return valueType;
}

shared_ptr<ValueType> ValueType::blob(string blobName) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::BLOB;
    valueType->blobName = blobName;
    return valueType;
}

shared_ptr<ValueType> ValueType::fun(vector<shared_ptr<ValueType>> argumentTypes, shared_ptr<ValueType> returnType) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::FUN;
    valueType->argumentTypes = argumentTypes;
    if (returnType != nullptr)
        valueType->returnType = returnType;
    else
        valueType->returnType = ValueType::NONE;
    return valueType;
}

shared_ptr<ValueType> ValueType::ptr(shared_ptr<ValueType> subType) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::PTR;
    valueType->subType = subType;
    return valueType;
}

shared_ptr<ValueType> ValueType::composite(vector<shared_ptr<ValueType>> elementTypes, shared_ptr<Expression> countExpression) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::COMPOSITE;
    valueType->compositeElementTypes = elementTypes;
    valueType->countExpression = countExpression;
    return valueType;
}

ValueType::ValueType() { }
ValueType::ValueType(ValueTypeKind kind):
kind(kind) { }

ValueTypeKind ValueType::getKind() {
    return kind;
}

shared_ptr<ValueType> ValueType::getSubType() {
    return subType;
}

int ValueType::getValueArg() {
    shared_ptr<ExpressionLiteral> expressionLiteral = dynamic_pointer_cast<ExpressionLiteral>(countExpression);
    if (expressionLiteral != nullptr)
        return expressionLiteral->getUIntValue();
    else
        return 0;
}

shared_ptr<Expression> ValueType::getCountExpression() {
    return countExpression;
}

optional<vector<shared_ptr<ValueType>>> ValueType::getArgumentTypes() {
    return argumentTypes;
}

shared_ptr<ValueType> ValueType::getReturnType() {
    return returnType;
}

optional<string> ValueType::getBlobName() {
    return blobName;
}

optional<vector<shared_ptr<ValueType>>> ValueType::getCompositeElementTypes() {
    return compositeElementTypes;
}

bool ValueType::isEqual(shared_ptr<ValueType> other) {
    if (other == nullptr)
        return false;

    switch (kind) {
        case ValueTypeKind::PTR: {
            return other->isPointer() && subType->isEqual(other->getSubType());
        }
        case ValueTypeKind::DATA: {
            // first check the types
            if (!other->isData() || !subType->isEqual(other->getSubType()))
                return false;

            // then check the elements count
            shared_ptr<ExpressionLiteral> thisCountLiteralExpression = dynamic_pointer_cast<ExpressionLiteral>(countExpression);
            shared_ptr<ExpressionLiteral> thatCountLiteralExpression = dynamic_pointer_cast<ExpressionLiteral>(other->getCountExpression());

            // if both have no size specified, then it's good
            if (thisCountLiteralExpression == nullptr && thatCountLiteralExpression == nullptr)
                return true;

            // otherwise check that both have some sizes
            if (thisCountLiteralExpression == nullptr || thatCountLiteralExpression == nullptr)
                return false;
            // sizes must be unsigned integers
            bool isThisTypeValid = thisCountLiteralExpression->getValueType()->isUnsignedInteger();
            bool isThatTypeValid = thatCountLiteralExpression->getValueType()->isUnsignedInteger();
            if (!isThisTypeValid || !isThatTypeValid)
                return false;

            int thisSize = thisCountLiteralExpression->getUIntValue();
            int thatSize = thatCountLiteralExpression->getUIntValue();

            return thisSize == thatSize;
        }
        case ValueTypeKind::BLOB: {
            if (!other->isBlob())
                return false;
            string tbn = *blobName;
            string obn = *other->getBlobName();
            return (*blobName).compare(*other->getBlobName()) == 0;
        }
        case ValueTypeKind::FUN: {
            // are both function types?
            if (!other->isFunction())
                return false;

            // does argument count match?
            if ((*argumentTypes).size() != (*other->getArgumentTypes()).size())
                return false;

            // do argument types match?
            for (int i=0; i<(*argumentTypes).size(); i++) {
                if (!(*argumentTypes).at(i)->isEqual((*other->getArgumentTypes()).at(i)))
                    return false;
            }

            // do the return types match?
            if (!returnType->isEqual(other->getReturnType()))
                return false;

            return true;
        }
        default:
            break;
    }
    return kind == other->getKind();
}

bool ValueType::isNumeric() {
    switch (kind) {
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:
    
        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64:

        case ValueTypeKind::A:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isInteger() {
    switch (kind) {
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::A:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isUnsignedInteger() {
    switch (kind) {
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:
        case ValueTypeKind::A:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isSignedInteger() {
    switch (kind) {
        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isFloat() {
    switch (kind) {
        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isBool() {
    return kind == ValueTypeKind::BOOL;
}

bool ValueType::isData() {
    return kind == ValueTypeKind::DATA;
}

bool ValueType::isDataBool() {
    if (isData() && getSubType()->isBool())
        return true;

    if (kind == ValueTypeKind::COMPOSITE) {
        vector<shared_ptr<ValueType>> elementTypes = *(getCompositeElementTypes());
        for (shared_ptr<ValueType> elementType : elementTypes) {
            if (!elementType->isBool())
                return false;
        }
        return true;
    }

    return false;
}

bool ValueType::isDataNumeric() {
    if (isData() && getSubType()->isNumeric())
        return true;

    if (kind == ValueTypeKind::COMPOSITE) {
        vector<shared_ptr<ValueType>> elementTypes = *(getCompositeElementTypes());
        for (shared_ptr<ValueType> elementType : elementTypes) {
            if (!elementType->isNumeric())
                return false;
        }
        return true;
    }

    return false;
}

bool ValueType::isAddress() {
    return kind == ValueTypeKind::A;
}

bool ValueType::isPointer() {
    return kind == ValueTypeKind::PTR;
}

bool ValueType::isFunction() {
    return kind == ValueTypeKind::FUN;
}

bool ValueType::isBlob() {
    return kind == ValueTypeKind::BLOB;
}
