#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include "Statement.h"
#include "Parser/SymbolName.h"
#include "Parser/ValueType/ValueType.h"

class Expression;

struct EnumField {
    std::shared_ptr<SymbolName> symbolName;
    std::shared_ptr<ValueType> payloadValueType;
    std::shared_ptr<Expression> tagExpression;
};

class StatementEnum: public Statement {
friend class Analyzer;

public:
    StatementEnum(
        bool shouldExport,
        const std::string &name,
        const std::vector<std::string> &namedValueTypeKeys,
        const std::vector<EnumField> &fields,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const std::string &moduleName);
    std::vector<std::string> getNamedValueTypeKeys() const;
    std::vector<EnumField> getFields() const;

private:
    bool shouldExport;
    std::shared_ptr<SymbolName> symbolName;
    std::vector<std::string> namedValueTypeKeys;
    std::vector<EnumField> fields;
};

#endif