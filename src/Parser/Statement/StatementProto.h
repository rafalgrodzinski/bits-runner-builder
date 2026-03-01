#ifndef STATEMENT_PROTO_H
#define STATEMENT_PROTO_H

#include "Statement.h"


class StatementVariable;
class StatementFunctionDeclaration;

class StatementProto: public Statement {
private:
    bool shouldExport;
    string name;
    vector<shared_ptr<StatementVariable>> variableStatements;
    vector<shared_ptr<StatementFunctionDeclaration>> functionDeclarationStatements;

public:
    StatementProto(
        bool shouldExport,
        string name,
        vector<shared_ptr<StatementVariable>> variableStatements,
        vector<shared_ptr<StatementFunctionDeclaration>> functionDeclarationStatements,
        shared_ptr<Location> location
    );
    bool getShouldExport();
    string getName();
    vector<shared_ptr<StatementVariable>> getVariableStatements();
    vector<shared_ptr<StatementFunctionDeclaration>> getFunctionDeclarationStatements();
};

#endif