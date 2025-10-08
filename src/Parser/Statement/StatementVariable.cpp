#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

StatementVariable::StatementVariable(bool shouldExport, string name, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression):
Statement(StatementKind::VARIABLE), shouldExport(shouldExport), name(name), valueType(valueType), expression(expression) { }

bool StatementVariable::getShouldExport() {
    return shouldExport;
}

string StatementVariable::getName() {
    return name;
}

shared_ptr<ValueType> StatementVariable::getValueType() {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() {
    return expression;
}
