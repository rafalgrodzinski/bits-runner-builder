#ifndef VALUE_TYPE_PTR_H
#define VALUE_TYPE_PTR_H

#include "ValueType.h"

class ValueTypePtr: public ValueType {
friend class Analyzer;
public:
    ValueTypePtr(
        std::shared_ptr<ValueType> pointeeValueType,
        bool isVolatile,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<ValueType> getPointeeValueType() const;
    bool getIsVolatile() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::shared_ptr<ValueType> pointeeValueType;
    bool isVolatile;
};

#endif