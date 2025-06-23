#include "StatementVariable.h"

#include "Parser/Expression.h"

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
Statement(StatementKind::VAR_DECLARATION), name(name), valueType(valueType), expression(expression) { }

string StatementVariable::getName() {
    return name;
}

ValueType StatementVariable::getValueType() {
    return valueType;
}

shared_ptr<Expression> StatementVariable::getExpression() {
    return expression;
}

string StatementVariable::toString(int indent) {
    string value;
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += name + "(";
    value += valueTypeToString(valueType);
    value += "):\n";
    for (int ind=0; ind<indent+1; ind++)
        value += "  ";
    value += expression->toString(indent+1);
    value += "\n";
    return value;
}