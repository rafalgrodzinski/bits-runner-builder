#include "StatementRawFunction.h"

StatementRawFunction::StatementRawFunction(string name, string rawSource):
Statement(StatementKind::RAW_FUNCTION), name(name), rawSource(rawSource) { }

string StatementRawFunction::getName() {
    return name;
}

string StatementRawFunction::getRawSource() {
    return rawSource;
}