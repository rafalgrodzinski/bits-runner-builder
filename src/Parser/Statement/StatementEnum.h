#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include "Statement.h"
#include "Parser/SymbolName.h"
#include "Parser/ValueType/ValueType.h"

class Expression;

struct EnumField {
    shared_ptr<SymbolName> symbolName;
    shared_ptr<ValueType> payloadValueType;
    shared_ptr<Expression> tagExpression;
};

class StatementEnum: public Statement {
friend class Analyzer;

public:
    StatementEnum(
        bool shouldExport,
        const string &name,
        const vector<string> &namedValueTypeKeys,
        const vector<EnumField> &fields,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const string &moduleName);
    vector<string> getNamedValueTypeKeys() const;
    vector<EnumField> getFields() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
    vector<string> namedValueTypeKeys;
    vector<EnumField> fields;
};

#endif