#include "StatementBlob.h"
#include "StatementBlobDeclaration.h"
#include "Parser/Statement/StatementVariableDeclaration.h"
#include "Parser/SymbolName.h"

StatementBlob::StatementBlob(
    bool shouldExport,
    const string &name,
    const vector<string> &namedTypeKeys,
    const vector<string> &protoNames,
    const vector<shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
    const vector<shared_ptr<StatementFunction>> &statementFunctions,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location),
shouldExport(shouldExport),
symbolName(make_shared<SymbolName>(name)),
namedTypeKeys(namedTypeKeys),
statementVariableDeclarations(statementVariableDeclarations),
statementFunctions(statementFunctions) {
    for (const string &protoName : protoNames) {
        protoSymbolNames.push_back(make_shared<SymbolName>(protoName));
    }
}

StatementBlob::StatementBlob(
    bool shouldExport,
    shared_ptr<SymbolName> symbolName,
    const vector<string> &namedTypeKeys,
    vector<shared_ptr<SymbolName>> conformingProtoSymbolNames,
    const vector<shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
    const vector<shared_ptr<StatementFunction>> &statementFunctions,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location),
shouldExport(shouldExport),
symbolName(symbolName),
namedTypeKeys(namedTypeKeys),
protoSymbolNames(conformingProtoSymbolNames),
statementVariableDeclarations(statementVariableDeclarations),
statementFunctions(statementFunctions) { }

bool StatementBlob::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementBlob::getSymbolName() const {
    return symbolName;
}

shared_ptr<SymbolName> StatementBlob::getPackedSymbolName() const {
    string packedName = format("{}_packed", symbolName->getName());
    return make_shared<SymbolName>(packedName, symbolName->getModuleName());
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

vector<shared_ptr<StatementVariableDeclaration>> StatementBlob::getStatementVariableDeclarations() const {
    return statementVariableDeclarations;
}

vector<shared_ptr<StatementFunction>> StatementBlob::getStatementFunctions() const {
    return statementFunctions;
}

vector<pair<string, shared_ptr<ValueType>>> StatementBlob::getMembers() const {
    vector<pair<string, shared_ptr<ValueType>>> members;

    for (shared_ptr<StatementVariableDeclaration> statement : statementVariableDeclarations)
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