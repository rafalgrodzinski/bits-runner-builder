#include "StatementEnum.h"

StatementEnum::StatementEnum(
    bool shouldExport,
    const string &name,
    shared_ptr<Location> Location
):
Statement(StatementKind::ENUM, Location),
shouldExport(shouldExport),
name(name) {
}

bool StatementEnum::getShouldExport() const {
    return shouldExport;
}

string StatementEnum::getName() const {
    return name;
}

string StatementEnum::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string StatementEnum::getModuleName() const {
    return moduleName;
}

void StatementEnum::setModuleName(const string &moduleName) {
    this->moduleName = moduleName;
}