#ifndef STATEMENT_MODULE_H
#define STATEMENT_MODULE_H

#include "Statement.h"

class StatementModule: public Statement {
public:
    StatementModule(const string &name, shared_ptr<Location> location);

    string getName() const;

private:
    string name;
};

#endif