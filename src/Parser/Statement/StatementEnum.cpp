#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    const vector<string> &namedTypeKeys,
    const vector<EnumField> &fields,
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

void StatementEnum::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
    for (EnumField &field : fields)
        field.symbolName->setModuleName(moduleName);
}

vector<string> StatementEnum::getNamedTypeKeys() const {
    return namedTypeKeys;
}

vector<EnumField> StatementEnum::getFields() const {
    return fields;
}

shared_ptr<ValueType> StatementEnum::getValueType() const {
    return ValueType::enumeration(symbolName->getGlobalName(), {});
};