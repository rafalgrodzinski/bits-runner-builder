#include "StatementRawFunction.h"

StatementRawFunction::StatementRawFunction(string name, string constraints, string rawSource):
Statement(StatementKind::RAW_FUNCTION), name(name), constraints(constraints), rawSource(rawSource) { }

string StatementRawFunction::getName() {
    return name;
}

string StatementRawFunction::getConstraints() {
    return constraints;
}

string StatementRawFunction::getRawSource() {
    return rawSource;
}