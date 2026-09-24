#ifndef VALUE_TYPE_COMPOSITE_H
#define VALUE_TYPE_COMPOSITE_H

#include <vector>

#include "ValueType.h"

class Expression;

class ValueTypeComposite: public ValueType {
public:
    ValueTypeComposite(
        const std::vector<std::shared_ptr<ValueType>> &elementValueTypes,
        std::shared_ptr<Expression> countExpression,
        std::shared_ptr<Location> location
    );

    std::vector<std::shared_ptr<ValueType>> getElementValueTypes() const;
    std::shared_ptr<Expression> getCountExpression() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::vector<std::shared_ptr<ValueType>> elementValueTypes;
    std::shared_ptr<Expression> countExpression;
};

#endif