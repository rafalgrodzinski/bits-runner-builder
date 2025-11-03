#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class StatementBlobDeclaration: public Statement {
private:
    string name;

public:
    StatementBlobDeclaration(string name);
    string getName();
};

#endif