#include "StatementProto.h"
#include "StatementProtoDeclaration.h"
#include "Parser/SymbolName.h"

StatementProto::StatementProto(
    bool shouldExport,
    const string &name,
    const vector<shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
    const vector<shared_ptr<StatementFunctionDeclaration>> &statementFunctionDeclarations,
    shared_ptr<Location> location
):
Statement(StatementKind::PROTO, location),
shouldExport(shouldExport),
symbolName(make_shared<SymbolName>(name)),
statementVariableDeclarations(statementVariableDeclarations),
statementFunctionDeclarations(statementFunctionDeclarations) { }

bool StatementProto::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementProto::getSymbolName() const {
    return symbolName;
}

void StatementProto::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
}

vector<shared_ptr<StatementVariableDeclaration>> StatementProto::getStatementVariableDeclarations() const {
    return statementVariableDeclarations;
}

vector<shared_ptr<StatementFunctionDeclaration>> StatementProto::getStatementFunctionDeclarations() const {
    return statementFunctionDeclarations;
}

shared_ptr<StatementProtoDeclaration> StatementProto::getDeclaration() const {
    return make_shared<StatementProtoDeclaration>(
        shouldExport,
        symbolName,
        getLocation()
    );
}