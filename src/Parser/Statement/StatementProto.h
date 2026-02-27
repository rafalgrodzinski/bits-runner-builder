#ifndef STATEMENT_PROTO_H
#define STATEMENT_PROTO_H

#include "Statement.h"


class StatementFunction;
class StatementVariable;

class StatementProto: public Statement {
private:
    bool shouldExport;
    string name;
    vector<shared_ptr<StatementVariable>> variableStatements;
    vector<shared_ptr<StatementFunction>> functionStatements;

public:
    StatementProto(
        bool shouldExport,
        string name,
        vector<shared_ptr<StatementVariable>> variableStatements,
        vector<shared_ptr<StatementFunction>> functionStatements,
        shared_ptr<Location> location
    );
    bool getShouldExport();
    string getName();
    vector<shared_ptr<StatementVariable>> getVariableStatements();
    vector<shared_ptr<StatementFunction>> getFunctionStatements();
};

#endif