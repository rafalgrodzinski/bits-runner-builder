#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    shared_ptr<Location> Location
):
Statement(StatementKind::ENUM, Location),
shouldExport(shouldExport) {
    symbolName = make_shared<SymbolName>(name);
}

bool StatementEnum::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementEnum::getSymbolName() const {
    return symbolName;
}