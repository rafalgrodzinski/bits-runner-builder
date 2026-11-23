#include "StatementProto.h"

#include "StatementProtoDeclaration.h"
#include "Parser/SymbolName.h"

using namespace std;

// MARK: - Public

StatementProto::StatementProto(
    bool isVisibleOutside,
    const string &name,
    const vector<shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
    const vector<shared_ptr<StatementFunctionDeclaration>> &statementFunctionDeclarations,
    shared_ptr<Location> location
):
Statement(StatementKind::PROTO, location),
isVisibleOutside(isVisibleOutside),
symbolName(make_shared<SymbolName>(name)),
statementVariableDeclarations(statementVariableDeclarations),
statementFunctionDeclarations(statementFunctionDeclarations) { }

bool StatementProto::getIsVisibleOutside() const {
    return isVisibleOutside;
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
        isVisibleOutside,
        symbolName,
        getLocation()
    );
}