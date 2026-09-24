#ifndef VALUE_TYPE_BLOB_H
#define VALUE_TYPE_BLOB_H

#include <optional>
#include <vector>
#include "ValueType.h"

class SymbolName;

class ValueTypeBlob: public ValueType {
friend class Analyzer;

public:
    ValueTypeBlob(
        const std::string &name,
        const std::vector<std::shared_ptr<ValueType>> &namedValueTypes,
        bool isPacked,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<SymbolName> getSymbolName() const;
    std::shared_ptr<SymbolName> getPackedSymbolName() const;
    std::optional<std::vector<std::string>> getNamedValueTypeKeys() const;
    std::vector<std::shared_ptr<ValueType>> getNamedValueTypes() const;
    bool getIsPacked() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::shared_ptr<SymbolName> symbolName;
    std::optional<std::vector<std::string>> namedValueTypeKeys;
    std::vector<std::shared_ptr<ValueType>> namedValueTypes;
    bool isPacked;
};

#endif