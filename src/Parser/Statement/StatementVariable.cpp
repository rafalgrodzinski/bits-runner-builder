#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

StatementVariable::StatementVariable(bool shouldExport, const string &identifier, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression, shared_ptr<Location> location):
Statement(StatementKind::VARIABLE, location), shouldExport(shouldExport), identifier(std::move(identifier)), valueType(valueType), expression(expression) { }

bool StatementVariable::getShouldExport() const {
    return shouldExport;
}

string StatementVariable::getIdentifier() const {
    return identifier;
}

shared_ptr<ValueType> StatementVariable::getValueType() const {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() const {
    return expression;
}
