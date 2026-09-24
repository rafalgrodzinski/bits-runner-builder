#ifndef VALUE_TYPE_BOXED_H
#define VALUE_TYPE_BOXED_H

#include <optional>
#include "ValueType.h"

class ValueTypeBoxed: public ValueType {
friend class Analyzer;

public:
    ValueTypeBoxed(
        const std::optional<std::string> &namedValueTypeKey,
        std::shared_ptr<ValueType> boxedValueType,
        std::shared_ptr<Location> location
    );

    std::optional<std::string> getNamedValueTypeKey() const;
    std::shared_ptr<ValueType> getBoxedValueType() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::optional<std::string> namedValueTypeKey;
    std::shared_ptr<ValueType> boxedValueType;
};

#endif