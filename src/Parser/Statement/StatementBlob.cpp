#include "StatementBlob.h"

#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/ValueType/ValueType.h"

StatementBlob::StatementBlob(
    bool shouldExport,
    const string &name,
    vector<string> namedTypeKeys,
    vector<string> protoNames,
    vector<shared_ptr<StatementVariable>> variableStatements,
    vector<shared_ptr<StatementFunction>> functionStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location), shouldExport(shouldExport), namedTypeKeys(std::move(namedTypeKeys)), protoNames(std::move(protoNames)), variableStatements(std::move(variableStatements)), functionStatements(std::move(functionStatements)) {
    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->moduleName = name.substr(0, pos);
        this->name = name.substr(pos + 1, name.size());
    } else {
        this->name = name;
    }
}

bool StatementBlob::getShouldExport() const {
    return shouldExport;
}

string StatementBlob::getName() const {
    return name;
}

string StatementBlob::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string StatementBlob::getModuleName() const {
    return moduleName;
}

void StatementBlob::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
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

shared_ptr<StatementBlobDeclaration> StatementBlob::getDeclaration() const {
    return make_shared<StatementBlobDeclaration>(
        getShouldExport(),
        getName(),
        getModuleName(),
        getLocation()
    );
}