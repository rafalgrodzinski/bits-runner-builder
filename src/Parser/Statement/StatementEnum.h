#ifndef STATEMENT_ENUM_H
#define STATEMENT_ENUM_H

#include <format>

#include "Statement.h"

class StatementEnum: public Statement {
private:
    bool shouldExport;
    string name;
    string moduleName;

public:
    StatementEnum(
        bool shouldExport,
        const string &name,
        shared_ptr<Location> Location
    );

    bool getShouldExport() const;
    string getName() const;
    string getGlobalName() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);
};

#endif