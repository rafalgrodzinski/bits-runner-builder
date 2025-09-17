#ifndef STATEMENT_BLOB_DECLARATION_H
#define STATEMENT_BLOB_DECLARATION_H

#include "Statement.h"

class StatementBlobDeclaration: public Statement {
private:
    string identifier;

public:
    StatementBlobDeclaration(string identifier);
    string getIdentifier();
};

#endif