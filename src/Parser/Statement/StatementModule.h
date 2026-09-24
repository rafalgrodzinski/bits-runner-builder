#ifndef STATEMENT_MODULE_H
#define STATEMENT_MODULE_H

#include "Statement.h"

class StatementModule: public Statement {
public:
    StatementModule(const std::string &name, std::shared_ptr<Location> location);

    std::string getName() const;

private:
    std::string name;
};

#endif