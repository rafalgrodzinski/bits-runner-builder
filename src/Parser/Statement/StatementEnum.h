#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include "Statement.h"
#include "Parser/SymbolName.h"

class StatementEnum: public Statement {
public:
    StatementEnum(
        bool shouldExport,
        const string &name,
        shared_ptr<Location> Location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
};

#endif