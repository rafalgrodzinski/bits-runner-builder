#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include "Statement.h"

#include "Parser/Field.h"
#include "Parser/SymbolName.h"
#include "Parser/ValueType.h"

class StatementEnum: public Statement {
public:
    StatementEnum(
        bool shouldExport,
        const string &name,
        const vector<string> &namedTypeKeys,
        const vector<Field> &fields,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;
    vector<string> getNamedTypeKeys() const;
    vector<Field> getFields() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
    vector<string> namedTypeKeys;
    vector<Field> fields;
};

#endif