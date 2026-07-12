#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include "Statement.h"

#include "Parser/SymbolName.h"
#include "Parser/ValueType.h"

struct EnumField {
    string name;
    shared_ptr<Expression> valueExpression;
    shared_ptr<ValueType> payloadValueType;
};

class StatementEnum: public Statement {
public:
    StatementEnum(
        bool shouldExport,
        const string &name,
        const vector<string> &namedTypeKeys,
        const vector<EnumField> &fields,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;
    vector<string> getNamedTypeKeys() const;
    vector<EnumField> getFields() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
    vector<string> namedTypeKeys;
    vector<EnumField> fields;
};

#endif