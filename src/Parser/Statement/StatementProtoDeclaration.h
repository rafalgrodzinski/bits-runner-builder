#ifndef STATEMENT_PROTO_DECLARATION_H
#define STATEMENT_PROTO_DECLARATION_H

#include "Statement.h"

class SymbolName;

class StatementProtoDeclaration: public Statement {
public:
    StatementProtoDeclaration(
        bool isVisibleOutside,
        std::shared_ptr<SymbolName> symbolName,
        std::shared_ptr<Location> location
    );

    bool getIsVisibleOutside() const;
    std::shared_ptr<SymbolName> getSymbolName() const;

private:
    bool isVisibleOutside;
    std::shared_ptr<SymbolName> symbolName;
};

#endif