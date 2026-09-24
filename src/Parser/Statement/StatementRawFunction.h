#ifndef STATEMENT_FUNCTION
#define STATEMENT_FUNCTION

#include <format>

#include "Parser/Statement/Statement.h"

class ValueType;
class ValueTypeFun;

class StatementRawFunction: public Statement {
public:
    StatementRawFunction(
        bool shouldExport,
        const std::string &name,
        const std::string &constraints,
        const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &arguments,
        std::shared_ptr<ValueType> returnValueType,
        const std::string &rawSource,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::string getName() const;
    std::string getGlobalName() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    std::string getConstraints() const;
    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> getArguments() const;
    std::shared_ptr<ValueType> getReturnValueType() const;
    std::shared_ptr<ValueTypeFun> getValueType() const;
    std::string getRawSource() const;

private:
    bool shouldExport;
    std::string name;
    std::string moduleName;
    std::string constraints;
    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> arguments;
    std::shared_ptr<ValueType> returnValueType;
    std::string rawSource;
};

#endif