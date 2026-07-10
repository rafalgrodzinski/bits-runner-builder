#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    const vector<string> &namedTypeKeys,
    const vector<Field> &fields,
    shared_ptr<Location> location
):
Statement(StatementKind::ENUM, location),
shouldExport(shouldExport),
namedTypeKeys(namedTypeKeys),
fields(fields) {
    symbolName = make_shared<SymbolName>(name);
}

bool StatementEnum::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementEnum::getSymbolName() const {
    return symbolName;
}

vector<string> StatementEnum::getNamedTypeKeys() const {
    return namedTypeKeys;
}

vector<Field> StatementEnum::getFields() const {
    return fields;
}