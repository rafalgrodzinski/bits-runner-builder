#ifndef VALUE_TYPE_DATA_H
#define VALUE_TYPE_DATA_H

#include "ValueType.h"

class Expression;

class ValueTypeData: public ValueType {
public:
    ValueTypeData(
        std::shared_ptr<ValueType> elementValueType,
        std::shared_ptr<Expression> countExpression,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<ValueType> getElementValueType() const;
    std::shared_ptr<Expression> getCountExpression() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::shared_ptr<ValueType> elementValueType;
    std::shared_ptr<Expression> countExpression;
};

#endif