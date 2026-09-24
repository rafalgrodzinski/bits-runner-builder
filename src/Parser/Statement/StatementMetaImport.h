#ifndef STATEMENT_IMPORT_H
#define STATEMENT_IMPORT_H

#include "Statement.h"

class StatementMetaImport: public Statement {
public:
    StatementMetaImport(const string &name, shared_ptr<Location> location);

    string getName() const;

private:
    string name;
};

#endif