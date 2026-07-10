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
        vector<Field> fields,
        shared_ptr<Location> Location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;
    vector<Field> getFields() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
    vector<Field> fields;
};

#endif