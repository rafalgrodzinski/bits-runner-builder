#include "StatementBlob.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/ValueType.h"

StatementBlob::StatementBlob(
    bool shouldExport,
    string name,
    vector<string> namedTypeKeys,
    vector<string> protoNames,
    vector<shared_ptr<StatementVariable>> variableStatements,
    vector<shared_ptr<StatementFunction>> functionStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location), shouldExport(shouldExport), name(std::move(name)), namedTypeKeys(std::move(namedTypeKeys)), protoNames(std::move(protoNames)), variableStatements(std::move(variableStatements)), functionStatements(std::move(functionStatements)) { }

bool StatementBlob::getShouldExport() const {
    return shouldExport;
}

string StatementBlob::getName() const {
    return name;
}

 vector<string> StatementBlob::getNamedTypeKeys() const {
    return namedTypeKeys;
 }

vector<string> StatementBlob::getProtoNames() const {
    return protoNames;
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