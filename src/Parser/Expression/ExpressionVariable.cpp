#include "ExpressionVariable.h"

ExpressionVariable::ExpressionVariable(string name):
Expression(ExpressionKind::VAR, nullptr), name(name) { }

string ExpressionVariable::getName() {
    return name;
}
