#ifndef STATEMENT_IMPORT_H
#define STATEMENT_IMPORT_H

#include "Statement.h"

class StatementMetaImport: public Statement {
public:
    StatementMetaImport(const std::string &name, std::shared_ptr<Location> location);

    std::string getName() const;

private:
    std::string name;
};

#endif