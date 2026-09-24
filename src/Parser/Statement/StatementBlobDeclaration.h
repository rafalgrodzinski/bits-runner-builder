#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class SymbolName;

class StatementBlobDeclaration: public Statement {
public:
    StatementBlobDeclaration(
        bool shouldExport,
        std::shared_ptr<SymbolName> symbolName,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::shared_ptr<SymbolName> getSymbolName() const;
    std::shared_ptr<SymbolName> getPackedSymbolName() const;

private:
    bool shouldExport;
    std::shared_ptr<SymbolName> symbolName;
};

#endif