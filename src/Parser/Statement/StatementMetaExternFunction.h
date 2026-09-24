#ifndef STATEMENT_META_EXTERN_FUNCTION_H
#define STATEMENT_META_EXTERN_FUNCTION_H

#include <format>

#include "Statement.h"

class ValueType;
class ValueTypeFun;

class StatementMetaExternFunction: public Statement {
public:
    StatementMetaExternFunction(
        const std::string &name,
        const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &arguments,
        std::shared_ptr<ValueType> returnValueType,
        std::shared_ptr<Location> location
    );

    std::string getName() const;
    std::string getGlobalName() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    std::string getSymbolName() const;

    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> getArguments() const;
    std::shared_ptr<ValueType> getReturnValueType() const;
    std::shared_ptr<ValueTypeFun> getValueType() const;

private:
    std::string name;
    std::string moduleName;
    std::string symbolName;

    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> arguments;
    std::shared_ptr<ValueType> returnValueType;
};

#endif