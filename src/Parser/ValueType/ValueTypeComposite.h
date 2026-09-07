#ifndef VALUE_TYPE_COMPOSITE_H
#define VALUE_TYPE_COMPOSITE_H

#include <vector>
#include "ValueType.h"

class Expression;

class ValueTypeComposite: public ValueType {
public:
    ValueTypeComposite(const vector<shared_ptr<ValueType>> &elementValueTypes, shared_ptr<Expression> countExpression);

    vector<shared_ptr<ValueType>> getElementValueTypes() const;
    shared_ptr<Expression> getCountExpression() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    vector<shared_ptr<ValueType>> elementValueTypes;
    shared_ptr<Expression> countExpression;
};

#endif