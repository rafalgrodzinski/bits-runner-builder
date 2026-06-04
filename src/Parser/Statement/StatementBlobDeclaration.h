#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class StatementBlobDeclaration: public Statement {
private:
    bool shouldExport;
    string name;

public:
    StatementBlobDeclaration(bool shouldExport, const string &name, shared_ptr<Location> location);
    bool getShouldExport() const;
    string getName() const;
};

#endif