#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

using namespace std;

// MARK: - Public

StatementVariable::StatementVariable(
    bool shouldExport,
    bool isPublic,
    VariableKind kind,
    const string &identifier,
    shared_ptr<ValueType> valueType,
    shared_ptr<Expression> expression,
    shared_ptr<Location> location
):
Statement(StatementKind::VARIABLE, location),
shouldExport(shouldExport),
isPublic(isPublic),
kind(kind),
identifier(identifier),
valueType(valueType),
expression(expression) { }

bool StatementVariable::getShouldExport() const {
    return shouldExport;
}

bool StatementVariable::getIsPublic() const {
    return isPublic;
}

VariableKind StatementVariable::getKind() const {
    return kind;
}

string StatementVariable::getIdentifier() const {
    return identifier;
}

string StatementVariable::getGlobalIdentifier() const {
    // local variables don't have module prefix
    if (!isRoot)
        return identifier;

    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, identifier);
}

string StatementVariable::getModuleName() const {
    return moduleName;
}

void StatementVariable::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

bool StatementVariable::getIsRoot() const {
    return isRoot;
}

void StatementVariable::setIsRoot(bool isRoot) {
    this->isRoot = isRoot;
}

shared_ptr<ValueType> StatementVariable::getValueType() const {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() const {
    return expression;
}

shared_ptr<StatementVariableDeclaration> StatementVariable::getDeclaration() const {
    return make_shared<StatementVariableDeclaration>(
        shouldExport,
        isPublic,
        kind,
        identifier,
        moduleName,
        isRoot,
        valueType,
        getLocation()
    );
}
