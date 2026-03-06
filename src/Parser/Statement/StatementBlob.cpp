#include "StatementBlob.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/ValueType.h"

StatementBlob::StatementBlob(
    bool shouldExport,
    string name,
    vector<string> protoNames,
    vector<shared_ptr<StatementVariable>> variableStatements,
    vector<shared_ptr<StatementFunction>> functionStatements,
    shared_ptr<Location> location
) :
Statement(StatementKind::BLOB, location), shouldExport(shouldExport), name(name), protoNames(protoNames), variableStatements(variableStatements), functionStatements(functionStatements) { }

bool StatementBlob::getShouldExport() {
    return shouldExport;
}

string StatementBlob::getName() {
    return name;
}

vector<string> StatementBlob::getProtoNames() {
    return protoNames;
}

vector<shared_ptr<StatementVariable>> StatementBlob::getVariableStatements() {
    return variableStatements;
}

vector<shared_ptr<StatementFunction>> StatementBlob::getFunctionStatements() {
    return functionStatements;
}

vector<pair<string, shared_ptr<ValueType>>> StatementBlob::getMembers() {
    vector<pair<string, shared_ptr<ValueType>>> members;

    for (shared_ptr<StatementVariable> statement : variableStatements)
        members.push_back(pair(statement->getIdentifier(), statement->getValueType()));

    return members;
}