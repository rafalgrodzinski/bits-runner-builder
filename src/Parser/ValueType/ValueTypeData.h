#ifndef VALUE_TYPE_DATA_H
#define VALUE_TYPE_DATA_H

#include "ValueType.h"

class ValueTypeData: public ValueType {
public:
    ValueTypeData(shared_ptr<ValueType> elementValueType, shared_ptr<Expression> countExpression);

    shared_ptr<ValueType> getElementValueType() const;
    shared_ptr<Expression> getCountExpression() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    shared_ptr<ValueType> elementValueType;
    shared_ptr<Expression> countExpression;
};

#endif