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

shared_ptr<ValueType> ValueType::blob(const string &blobName, const optional<vector<shared_ptr<ValueType>>> &namedTypeValues) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>(ValueTypeKind::BLOB, blobName);
    valueType->namedTypeValues = namedTypeValues;
    return valueType;
}

shared_ptr<ValueType> ValueType::proto(const string &protoName) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>(ValueTypeKind::PROTO, protoName);
    return valueType;
}

shared_ptr<ValueType> ValueType::fun(const vector<shared_ptr<ValueType>> &argumentTypes, shared_ptr<ValueType> returnType) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::FUN;
    valueType->argumentTypes = argumentTypes;
    if (returnType != nullptr)
        valueType->returnType = returnType;
    else
        valueType->returnType = ValueType::NONE;
    return valueType;
}

shared_ptr<ValueType> ValueType::ptr(shared_ptr<ValueType> subType, bool isVolatile) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::PTR;
    valueType->subType = subType;
    valueType->isVolatile = isVolatile;
    return valueType;
}

shared_ptr<ValueType> ValueType::composite(const vector<shared_ptr<ValueType>> &elementTypes, shared_ptr<Expression> countExpression) {
    shared_ptr<ValueType> valueType = make_shared<ValueType>();
    valueType->kind = ValueTypeKind::COMPOSITE;
    valueType->compositeElementTypes = elementTypes;
    valueType->countExpression = countExpression;
    return valueType;
}

ValueType::ValueType() { }

ValueType::ValueType(ValueTypeKind kind, const string &name):
kind(kind) {
    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->moduleName = name.substr(0, pos);
        this->name = name.substr(pos + 1, name.length());
    } else {
        this->name = name;
    }
}

ValueTypeKind ValueType::getKind() const {
    return kind;
}

string ValueType::getName() const {
    return name;
}

string ValueType::getModuleName() const {
    return moduleName;
}

void ValueType::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;

    if (this->getSubType() != nullptr)
        this->getSubType()->setModuleName(moduleName);

    if (argumentTypes) {
        for (shared_ptr<ValueType> typeValue : *argumentTypes)
            typeValue->setModuleName(moduleName);
    }

    if (namedTypeValues) {
        for (shared_ptr<ValueType> typeValue : *namedTypeValues)
            typeValue->setModuleName(moduleName);
    }

    if (returnType != nullptr) {
        returnType->setModuleName(moduleName);
    }
}

string ValueType::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

bool ValueType::getIsVolatile() const {
    return isVolatile;
}

shared_ptr<ValueType> ValueType::getSubType() const {
    if (subType == nullptr)
        return nullptr;

    subType->isVolatile = isVolatile;

    if (kind == ValueTypeKind::BOXED) {
        subType->namedTypeKeys = namedTypeKeys;
        subType->namedTypeValues = namedTypeValues;
    }

    return subType;
}

int ValueType::getValueArg() {
    shared_ptr<ExpressionLiteral> expressionLiteral = dynamic_pointer_cast<ExpressionLiteral>(countExpression);
    if (expressionLiteral != nullptr)
        return expressionLiteral->getUIntValue();
    else
        return 0;
}

shared_ptr<Expression> ValueType::getCountExpression() const {
    return countExpression;
}

optional<vector<shared_ptr<ValueType>>> ValueType::getArgumentTypes() const {
    return argumentTypes;
}

shared_ptr<ValueType> ValueType::getReturnType() const {
    if (returnType == nullptr)
        return nullptr;
    returnType->namedTypeKeys = namedTypeKeys;
    returnType->namedTypeValues = namedTypeValues;
    return returnType;
}

optional<vector<shared_ptr<ValueType>>> ValueType::getCompositeElementTypes() const {
    return compositeElementTypes;
}

optional<string> ValueType::getNamedTypeKey() const {
    return namedTypeKey;
}

optional<vector<string>> ValueType::getNamedTypeKeys() const {
    return namedTypeKeys;
}

optional<vector<shared_ptr<ValueType>>> ValueType::getNamedTypeValues() const {
    return namedTypeValues;
}

bool ValueType::isEqual(shared_ptr<ValueType> other) const {
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
            return getGlobalName() == other->getGlobalName();
        }
        case ValueTypeKind::ENUM: {
            if (other->getKind() != ValueTypeKind::ENUM)
                return false;
            return getGlobalName() == other->getGlobalName();
        }
        case ValueTypeKind::BOXED: {
            return other->isBoxed() && subType->isEqual(other->getSubType());
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

bool ValueType::isNumeric() const {
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

bool ValueType::isInteger() const {
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

bool ValueType::isUnsignedInteger() const {
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

bool ValueType::isSignedInteger() const {
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

bool ValueType::isFloat() const {
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

bool ValueType::isBool() const {
    return kind == ValueTypeKind::BOOL;
}

bool ValueType::isData() const {
    return kind == ValueTypeKind::DATA;
}

bool ValueType::isDataBool() const {
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

bool ValueType::isDataNumeric() const {
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

bool ValueType::isAddress() const {
    return kind == ValueTypeKind::A;
}

bool ValueType::isPointer() const {
    return kind == ValueTypeKind::PTR;
}

bool ValueType::isFunction() const {
    return kind == ValueTypeKind::FUN;
}

bool ValueType::isBlob() const {
    return kind == ValueTypeKind::BLOB;
}

bool ValueType::isEnum() const {
    return kind == ValueTypeKind::ENUM || kind == ValueTypeKind::ENUM_FIELD;
}

bool ValueType::isProto() const {
    return kind == ValueTypeKind::PROTO;
}

bool ValueType::isBoxed() const {
    return kind == ValueTypeKind::BOXED;
}

bool ValueType::isComposite() const {
    return kind == ValueTypeKind::COMPOSITE;
}

bool ValueType::isNamedType() const {
    return kind == ValueTypeKind::NAMED_TYPE;
}

bool ValueType::isBoxedNamedType() const {
    return kind == ValueTypeKind::BOXED && subType->isNamedType();
}

void ValueType::setParent(weak_ptr<ValueType> parent) {
    this->parent = parent;
}

weak_ptr<ValueType> ValueType::getParent() {
    return parent;
}