#ifndef VALUE_TYPE_SIMPLE_H
#define VALUE_TYPE_SIMPLE_H

#include "ValueType.h"

class Token;

class ValueTypeSimple: public ValueType {
friend class ValueType;

public:
    static shared_ptr<ValueType> NONE;
    static shared_ptr<ValueType> BOOL;
    static shared_ptr<ValueType> UINT;
    static shared_ptr<ValueType> U8;
    static shared_ptr<ValueType> U16;
    static shared_ptr<ValueType> U32;
    static shared_ptr<ValueType> U64;
    static shared_ptr<ValueType> SINT;
    static shared_ptr<ValueType> S8;
    static shared_ptr<ValueType> S16;
    static shared_ptr<ValueType> S32;
    static shared_ptr<ValueType> S64;
    static shared_ptr<ValueType> FLOAT;
    static shared_ptr<ValueType> F32;
    static shared_ptr<ValueType> F64;
    static shared_ptr<ValueType> A;

    static shared_ptr<ValueTypeSimple> simpleForToken(shared_ptr<Token> token);

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

public:
    ValueTypeSimple(ValueTypeKind kind);
};

#endif