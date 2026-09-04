#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    const vector<string> &namedValueTypeKeys,
    const vector<EnumField> &fields,
    shared_ptr<Location> location
):
Statement(StatementKind::ENUM, location),
shouldExport(shouldExport),
symbolName(make_shared<SymbolName>(name)),
namedValueTypeKeys(namedValueTypeKeys),
fields(fields) { }

bool StatementEnum::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementEnum::getSymbolName() const {
    return symbolName;
}

void StatementEnum::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
    for (EnumField &field : fields)
        field.symbolName->setModuleName(moduleName);
}

vector<string> StatementEnum::getNamedValueTypeKeys() const {
    return namedValueTypeKeys;
}

vector<EnumField> StatementEnum::getFields() const {
    return fields;
}