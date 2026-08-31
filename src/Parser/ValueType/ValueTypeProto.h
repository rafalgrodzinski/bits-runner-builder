#ifndef VALUE_TYPE_PROTO_H
#define VALUE_TYPE_PROTO_H

#include "ValueType.h"
#include "Parser/SymbolName.h"

class ValueTypeProto: public ValueType {
public:
    ValueTypeProto(const string &name);

    shared_ptr<SymbolName> getSymbolName() const;

    void setModuleName(const string &moduleName) override;
    bool isEqual(shared_ptr<ValueType> other) const override;
    shared_ptr<ValueType> clone() const override;

private:
    shared_ptr<SymbolName> symbolName;
};

#endif