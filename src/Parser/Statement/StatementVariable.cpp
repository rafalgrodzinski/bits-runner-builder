#include "StatementVariable.h"

#include "Parser/Expression/Expression.h"

static string valueTypeToString(ValueType valueType) {
    switch (valueType) {
        case ValueType::NONE:
            return "NONE";
        case ValueType::BOOL:
            return "BOOL";
        case ValueType::SINT32:
            return "SINT32";
        case ValueType::REAL32:
            return "REAL32";
    }
}

StatementVariable::StatementVariable(string name, ValueType valueType, shared_ptr<Expression> expression):
Statement(StatementKind::VARIABLE), name(name), valueType(valueType), expression(expression) { }

string StatementVariable::getName() {
    return name;
}

ValueType StatementVariable::getValueType() {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() {
    return expression;
}
