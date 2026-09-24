#ifndef VALUE_TYPE_ENUM_FIELD_H
#define VALUE_TYPE_ENUM_FIELD_H

#include <map>
#include <vector>
#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeEnumField: public ValueType {
friend class Analyzer;

public:
    ValueTypeEnumField(
        const std::string &name,
        const std::vector<std::shared_ptr<ValueType>> namedValueTypes,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<SymbolName> getSymbolName() const;
    std::vector<std::shared_ptr<ValueType>> getNamedValueTypes() const;
    std::shared_ptr<ValueType> getPayloadValueType() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;
    
private:
    std::shared_ptr<SymbolName> symbolName;
    std::vector<std::string> namedValueTypeKeys;
    std::vector<std::shared_ptr<ValueType>> namedValueTypes;
    std::shared_ptr<ValueType> payloadValueType;
};

#endif