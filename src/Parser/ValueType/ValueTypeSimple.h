#ifndef VALUE_TYPE_SIMPLE_H
#define VALUE_TYPE_SIMPLE_H

#include "ValueType.h"

class Token;

class ValueTypeSimple: public ValueType {
friend class ValueType;

public:
    static std::shared_ptr<ValueType> NONE;
    static std::shared_ptr<ValueType> BOOL;
    static std::shared_ptr<ValueType> UINT;
    static std::shared_ptr<ValueType> U8;
    static std::shared_ptr<ValueType> U16;
    static std::shared_ptr<ValueType> U32;
    static std::shared_ptr<ValueType> U64;
    static std::shared_ptr<ValueType> SINT;
    static std::shared_ptr<ValueType> S8;
    static std::shared_ptr<ValueType> S16;
    static std::shared_ptr<ValueType> S32;
    static std::shared_ptr<ValueType> S64;
    static std::shared_ptr<ValueType> FLOAT;
    static std::shared_ptr<ValueType> F32;
    static std::shared_ptr<ValueType> F64;
    static std::shared_ptr<ValueType> A;

    static std::shared_ptr<ValueTypeSimple> simpleForToken(std::shared_ptr<Token> token, std::shared_ptr<Location> location);

    ValueTypeSimple(ValueTypeKind kind, std::shared_ptr<Location> location);

    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;
};

#endif