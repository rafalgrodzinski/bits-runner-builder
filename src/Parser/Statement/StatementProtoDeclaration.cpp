#include "StatementProtoDeclaration.h"

StatementProtoDeclaration::StatementProtoDeclaration(bool shouldExport, const string &name, const string &moduleName, shared_ptr<Location> location):
Statement(StatementKind::PROTO_DECLARATION, location), shouldExport(shouldExport), name(name), moduleName(moduleName) { }

bool StatementProtoDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementProtoDeclaration::getName() const {
    return name;
}

string StatementProtoDeclaration::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name); 
}

string StatementProtoDeclaration::getModuleName() const {
    return moduleName;
}
