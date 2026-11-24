#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

using namespace std;

// MARK: - Public

StatementVariable::StatementVariable(
    bool isVisibleOutside,
    VariableKind kind,
    const string &identifier,
    shared_ptr<ValueType> valueType,
    shared_ptr<Expression> expression,
    shared_ptr<Location> location
):
Statement(StatementKind::VARIABLE, location),
isVisibleOutside(isVisibleOutside),
kind(kind),
identifier(identifier),
valueType(valueType),
expression(expression) { }

bool StatementVariable::getIsVisibleOutside() const {
    return isVisibleOutside;
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
        isVisibleOutside,
        kind,
        identifier,
        moduleName,
        isRoot,
        valueType,
        getLocation()
    );
}
