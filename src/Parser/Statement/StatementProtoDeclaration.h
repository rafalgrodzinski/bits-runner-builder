#ifndef STATEMENT_PROTO_DECLARATION_H
#define STATEMENT_PROTO_DECLARATION_H

#include "Statement.h"

class StatementProtoDeclaration: public Statement {
private:
    bool shouldExport;
    string name;
    string moduleName;

public:
    StatementProtoDeclaration(bool shouldExport, const string &name, const string &moduleName, shared_ptr<Location> location);

    bool getShouldExport() const;
    string getName() const;
    string getGlobalName() const;
    string getModuleName() const;
};

#endif