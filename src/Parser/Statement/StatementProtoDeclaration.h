#ifndef STATEMENT_PROTO_DECLARATION_H
#define STATEMENT_PROTO_DECLARATION_H

#include "Statement.h"

class SymbolName;

class StatementProtoDeclaration: public Statement {
public:
    StatementProtoDeclaration(
        bool shouldExport,
        std::shared_ptr<SymbolName> symbolName,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::shared_ptr<SymbolName> getSymbolName() const;

private:
    bool shouldExport;
    std::shared_ptr<SymbolName> symbolName;
};

#endif