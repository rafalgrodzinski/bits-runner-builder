#ifndef STATEMENT_PROTO_DECLARATION_H
#define STATEMENT_PROTO_DECLARATION_H

#include "Statement.h"

class StatementProtoDeclaration: public Statement {
private:
    bool shouldExport;
    string name;

public:
    StatementProtoDeclaration(bool shouldExport, string name, shared_ptr<Location> location);
    bool getShouldExport();
    string getName();
};

#endif