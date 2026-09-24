#ifndef VALUE_TYPE_ENUM_H
#define VALUE_TYPE_ENUM_H

#include <map>
#include <optional>
#include <vector>

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeEnum: public ValueType {
friend class Analyzer;

public:
    ValueTypeEnum(
        const std::string &name,
        const std::vector<std::shared_ptr<ValueType>> &namedValueTypes,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<SymbolName> getSymbolName() const;
    std::vector<std::shared_ptr<ValueType>> getNamedValueTypes();

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::shared_ptr<SymbolName> symbolName;
    std::optional<std::vector<std::string>> namedValueTypeKeys;
    std::vector<std::shared_ptr<ValueType>> namedValueTypes;
};

#endif