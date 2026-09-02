#include "StatementBlob.h"
#include "StatementBlobDeclaration.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/SymbolName.h"
#include "Parser/ValueType/ValueType.h"

StatementBlob::StatementBlob(
    bool shouldExport,
    const string &name,
    const vector<string> &namedTypeKeys,
    const vector<string> &protoNames,
    const vector<shared_ptr<StatementVariable>> &variableStatements,
    const vector<shared_ptr<StatementFunction>> &functionStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location),
shouldExport(shouldExport),
symbolName(make_shared<SymbolName>(name)),
namedTypeKeys(namedTypeKeys),
variableStatements(variableStatements),
functionStatements(functionStatements) {
    for (const string &protoName : protoNames) {
        protoSymbolNames.push_back(make_shared<SymbolName>(protoName));
    }
}

StatementBlob::StatementBlob(
    bool shouldExport,
    shared_ptr<SymbolName> symbolName,
    const vector<string> &namedTypeKeys,
    vector<shared_ptr<SymbolName>> conformingProtoSymbolNames,
    const vector<shared_ptr<StatementVariable>> &variableStatements,
    const vector<shared_ptr<StatementFunction>> &functionStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location),
shouldExport(shouldExport),
symbolName(symbolName),
namedTypeKeys(namedTypeKeys),
protoSymbolNames(conformingProtoSymbolNames),
variableStatements(variableStatements),
functionStatements(functionStatements) { }

bool StatementBlob::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementBlob::getSymbolName() const {
    return symbolName;
}

void StatementBlob::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);

    for (shared_ptr<SymbolName> protoSymbolName : protoSymbolNames)
        protoSymbolName->setModuleName(moduleName);
}

vector<string> StatementBlob::getNamedTypeKeys() const {
    return namedTypeKeys;
}

vector<shared_ptr<SymbolName>> StatementBlob::getProtoSymbolNames() const {
    return protoSymbolNames;
}

vector<shared_ptr<StatementVariable>> StatementBlob::getVariableStatements() const {
    return variableStatements;
}

vector<shared_ptr<StatementFunction>> StatementBlob::getFunctionStatements() const {
    return functionStatements;
}

vector<pair<string, shared_ptr<ValueType>>> StatementBlob::getMembers() const {
    vector<pair<string, shared_ptr<ValueType>>> members;

    for (shared_ptr<StatementVariable> statement : variableStatements)
        members.push_back(pair(statement->getIdentifier(), statement->getValueType()));

    return members;
}

shared_ptr<StatementBlobDeclaration> StatementBlob::getDeclaration() const {
    return make_shared<StatementBlobDeclaration>(
        shouldExport,
        symbolName,
        getLocation()
    );
}