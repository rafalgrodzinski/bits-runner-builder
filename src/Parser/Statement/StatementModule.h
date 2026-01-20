#ifndef STATEMENT_MODULE_H
#define STATEMENT_MODULE_H

#include "Statement.h"

class StatementModule: public Statement {
private:
    string name;

public:
    StatementModule(string name, shared_ptr<Location> location);
    string getName();
};

#endif