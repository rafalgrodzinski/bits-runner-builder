#ifndef VALUE_TYPE_FUN_H
#define VALUE_TYPE_FUN_H

#include <vector>

#include "ValueType.h"

class SymbolName;

class ValueTypeFun: public ValueType {
public:
    ValueTypeFun(
        const std::vector<std::shared_ptr<ValueType>> &argumentValueTypes,
        std::shared_ptr<ValueType> returnValueType,
        std::shared_ptr<Location> location
    );

    std::vector<std::shared_ptr<ValueType>> getArgumentValueTypes() const;
    std::shared_ptr<ValueType> getReturnValueType() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::shared_ptr<SymbolName> symbolName;
    std::vector<std::shared_ptr<ValueType>> argumentValueTypes;
    std::shared_ptr<ValueType> returnValueType;
};

#endif