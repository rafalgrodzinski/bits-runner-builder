#ifndef STATEMENT_PROTO_DECLARATION_H
#define STATEMENT_PROTO_DECLARATION_H

#include "Statement.h"

class SymbolName;

class StatementProtoDeclaration: public Statement {
public:
    StatementProtoDeclaration(
        bool shouldExport,
        shared_ptr<SymbolName> symbolName,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
};

#endif