#ifndef STATEMENT_PROTO_H
#define STATEMENT_PROTO_H

#include "Statement.h"

class StatementProtoDeclaration;
class StatementVariableDeclaration;
class StatementFunctionDeclaration;
class SymbolName;

class StatementProto: public Statement {
public:
    StatementProto(
        bool shouldExport,
        const string &name,
        const vector<shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
        const vector<shared_ptr<StatementFunctionDeclaration>> &statementFunctionDeclarations,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const string &moduleName);

    vector<shared_ptr<StatementVariableDeclaration>> getStatementVariableDeclarations() const;
    vector<shared_ptr<StatementFunctionDeclaration>> getStatementFunctionDeclarations() const;

    shared_ptr<StatementProtoDeclaration> getDeclaration() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
    vector<shared_ptr<StatementVariableDeclaration>> statementVariableDeclarations;
    vector<shared_ptr<StatementFunctionDeclaration>> statementFunctionDeclarations;
};

#endif