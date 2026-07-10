#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    vector<Field> fields,
    shared_ptr<Location> Location
):
Statement(StatementKind::ENUM, Location),
shouldExport(shouldExport),
fields(fields) {
    symbolName = make_shared<SymbolName>(name);
}

bool StatementEnum::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementEnum::getSymbolName() const {
    return symbolName;
}

vector<Field> StatementEnum::getFields() const {
    return fields;
}