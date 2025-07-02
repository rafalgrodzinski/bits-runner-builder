#include "ExpressionVariable.h"

ExpressionVariable::ExpressionVariable(string name):
Expression(ExpressionKind::VAR, ValueType::NONE), name(name) { }

string ExpressionVariable::getName() {
    return name;
}
