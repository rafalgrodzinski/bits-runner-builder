#ifndef STATEMENT_IMPORT_H
#define STATEMENT_IMPORT_H

#include "Statement.h"

class StatementImport: public Statement {
private:
    string name;

public:
    StatementImport(string name);
    string getName();
};

#endif