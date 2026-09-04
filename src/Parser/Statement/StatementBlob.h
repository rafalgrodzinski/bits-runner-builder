#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include <format>
#include "Statement.h"

class StatementBlobDeclaration;
class StatementFunction;
class StatementVariable;
class SymbolName;
class ValueType;

class StatementBlob: public Statement {
public:
    StatementBlob(
        bool shouldExport,
        const string &name,
        const vector<string> &namedTypeKeys,
        const vector<string> &protoNames,
        const vector<shared_ptr<StatementVariable>> &variableStatements,
        const vector<shared_ptr<StatementFunction>> &functionStatements,
        shared_ptr<Location> location
    );

    StatementBlob(
        bool shouldExport,
        shared_ptr<SymbolName> symbolName,
        const vector<string> &namedTypeKeys,
        vector<shared_ptr<SymbolName>> conformingProtoSymbolNames,
        const vector<shared_ptr<StatementVariable>> &variableStatements,
        const vector<shared_ptr<StatementFunction>> &functionStatements,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const string &moduleName);

    vector<string> getNamedTypeKeys() const;
    vector<shared_ptr<SymbolName>> getProtoSymbolNames() const;
    vector<shared_ptr<StatementVariable>> getVariableStatements() const;
    vector<shared_ptr<StatementFunction>> getFunctionStatements() const;
    vector<pair<string, shared_ptr<ValueType>>> getMembers() const;

    shared_ptr<StatementBlobDeclaration> getDeclaration() const;

private:
    bool shouldExport;
    shared_ptr<SymbolName> symbolName;
    vector<string> namedTypeKeys;
    vector<shared_ptr<StatementVariable>> variableStatements;
    vector<shared_ptr<StatementFunction>> functionStatements;
    vector<shared_ptr<SymbolName>> protoSymbolNames;
};

#endif