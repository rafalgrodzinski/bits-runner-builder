#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include "Statement.h"

class StatementEnum: public Statement {
private:
    bool shouldExport;

public:
    StatementEnum(
        bool shouldExport,
        const string &name,
        shared_ptr<Location> Location
    );
};

#endif