#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

StatementVariable::StatementVariable(string name, shared_ptr<ValueType> valueType, shared_ptr<Expression> expression):
Statement(StatementKind::VARIABLE), name(name), valueType(valueType), expression(expression) { }

string StatementVariable::getName() {
    return name;
}

shared_ptr<ValueType> StatementVariable::getValueType() {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() {
    return expression;
}
