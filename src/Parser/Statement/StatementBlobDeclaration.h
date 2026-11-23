#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class SymbolName;

class StatementBlobDeclaration: public Statement {
public:
    StatementBlobDeclaration(
        bool isVisibleOutside,
        std::shared_ptr<SymbolName> symbolName,
        std::shared_ptr<Location> location
    );

    bool getIsVisibleOutside() const;
    std::shared_ptr<SymbolName> getSymbolName() const;
    std::shared_ptr<SymbolName> getPackedSymbolName() const;

private:
    bool isVisibleOutside;
    std::shared_ptr<SymbolName> symbolName;
};

#endif