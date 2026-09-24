#ifndef VALUE_TYPE_PROTO_H
#define VALUE_TYPE_PROTO_H

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeProto: public ValueType {
public:
    ValueTypeProto(const std::string &name, std::shared_ptr<Location> location);

    std::shared_ptr<SymbolName> getSymbolName() const;

    void setModuleName(const std::string &moduleName) override;
    bool isEqual(std::shared_ptr<ValueType> other) const override;
    std::shared_ptr<ValueType> clone() const override;

private:
    std::shared_ptr<SymbolName> symbolName;
};

#endif