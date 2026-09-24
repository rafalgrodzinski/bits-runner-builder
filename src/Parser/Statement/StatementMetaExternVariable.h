#ifndef STATEMENT_META_EXTERN_VARIABLE_H
#define STATEMENT_META_EXTERN_VARIABLE_H

#include <format>

#include "Statement.h"

class ValueType;

class StatementMetaExternVariable: public Statement {
public:
    StatementMetaExternVariable(const std::string &identifier, std::shared_ptr<ValueType> valueType, std::shared_ptr<Location> location);

    std::string getIdentifier() const;
    std::string getGlobalIdentifier() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    std::string getSymbolName() const;
    std::shared_ptr<ValueType> getValueType() const;

private:
    std::string identifier;
    std::string moduleName;
    std::string symbolName;
    std::shared_ptr<ValueType> valueType;
};

#endif