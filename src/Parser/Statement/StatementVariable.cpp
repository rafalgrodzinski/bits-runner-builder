#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

StatementVariable::StatementVariable(bool shouldExport, string identifier, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression, int line, int column):
Statement(StatementKind::VARIABLE, line, column), shouldExport(shouldExport), identifier(identifier), valueType(valueType), expression(expression) { }

bool StatementVariable::getShouldExport() {
    return shouldExport;
}

string StatementVariable::getIdentifier() {
    return identifier;
}

shared_ptr<ValueType> StatementVariable::getValueType() {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() {
    return expression;
}
