#include "StatementVariableDeclaration.h"

using namespace std;

// MARK: - Public

StatementVariableDeclaration::StatementVariableDeclaration(
    bool shouldExport,
    bool isPublic,
    VariableKind kind,
    const string &identifier,
    const string &moduleName,
    bool isRoot,
    shared_ptr<ValueType> valueType,
    shared_ptr<Location> location
):
Statement(StatementKind::VARIABLE_DECLARATION, location),
shouldExport(shouldExport),
isPublic(isPublic),
kind(kind),
identifier(identifier),
moduleName(moduleName),
isRoot(isRoot),
valueType(valueType) { }

bool StatementVariableDeclaration::getShouldExport() const {
    return shouldExport;
}

bool StatementVariableDeclaration::getIsPublic() const {
    return isPublic;
}

VariableKind StatementVariableDeclaration::getKind() const {
    return kind;
}

string StatementVariableDeclaration::getIdentifier() const {
    return identifier;
}

string StatementVariableDeclaration::getGlobalIdentifier() const {
    // local variables don't have module prefix
    if (!isRoot)
        return identifier;

    return format("{}.{}", moduleName, identifier);
}

string StatementVariableDeclaration::getModuleName() const {
    return moduleName;
}

void StatementVariableDeclaration::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

bool StatementVariableDeclaration::getIsRoot() const {
    return isRoot;
}

shared_ptr<ValueType> StatementVariableDeclaration::getValueType() const {
    return valueType;
}