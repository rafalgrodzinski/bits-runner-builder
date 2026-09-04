#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class SymbolName;

class StatementBlobDeclaration: public Statement {
public:
    StatementBlobDeclaration(
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