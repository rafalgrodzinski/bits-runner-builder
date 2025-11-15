#ifndef STATEMENT_IMPORT_H
#define STATEMENT_IMPORT_H

#include "Statement.h"

class StatementMetaImport: public Statement {
private:
    string name;

public:
    StatementMetaImport(string name, int line, int column);
    string getName();
};

#endif