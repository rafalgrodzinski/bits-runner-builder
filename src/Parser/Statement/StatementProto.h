#ifndef STATEMENT_PROTO_H
#define STATEMENT_PROTO_H

#include "Statement.h"
#include "StatementProtoDeclaration.h"

class StatementVariable;
class StatementFunctionDeclaration;

class StatementProto: public Statement {
private:
    bool shouldExport;
    string name;
    string moduleName;

    vector<shared_ptr<StatementVariable>> variableStatements;
    vector<shared_ptr<StatementFunctionDeclaration>> functionDeclarationStatements;

public:
    StatementProto(
        bool shouldExport,
        const string &name,
        const vector<shared_ptr<StatementVariable>> &variableStatements,
        const vector<shared_ptr<StatementFunctionDeclaration>> &functionDeclarationStatements,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getName() const;
    string getGlobalName() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);

    vector<shared_ptr<StatementVariable>> getVariableStatements() const;
    vector<shared_ptr<StatementFunctionDeclaration>> getFunctionDeclarationStatements() const;

    shared_ptr<StatementProtoDeclaration> getDeclaration() const;
};

#endif