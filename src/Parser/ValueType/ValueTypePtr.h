#ifndef VALUE_TYPE_PTR_H
#define VALUE_TYPE_PTR_H

#include "ValueType.h"

class ValueTypePtr: public ValueType {
public:
    ValueTypePtr(shared_ptr<ValueType> pointeeValueType, bool isVolatile);

    shared_ptr<ValueType> getPointeeValueType() const;
    bool getIsVolatile() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    shared_ptr<ValueType> pointeeValueType;
    bool isVolatile;
};

#endif