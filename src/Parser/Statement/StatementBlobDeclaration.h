#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class StatementBlobDeclaration: public Statement {
private:
    bool shouldExport;
    string name;

public:
    StatementBlobDeclaration(bool shouldExport, string name, int line, int column);
    bool getShouldExport();
    string getName();
};

#endif