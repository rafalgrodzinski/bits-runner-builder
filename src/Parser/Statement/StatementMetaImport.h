#ifndef STATEMENT_IMPORT_H
#define STATEMENT_IMPORT_H

#include "Statement.h"

class StatementMetaImport: public Statement {
private:
    string name;

public:
    StatementMetaImport(string name, shared_ptr<Location> location);
    string getName();
};

#endif