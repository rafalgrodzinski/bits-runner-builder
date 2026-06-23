#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(bool shouldExport, const string &name, const string &moduleName, shared_ptr<Location> location):
Statement(StatementKind::BLOB_DECLARATION, location), shouldExport(shouldExport), name(name), moduleName(moduleName) { }

bool StatementBlobDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementBlobDeclaration::getName() const {
    return name;
}

string StatementBlobDeclaration::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name); 
}

string StatementBlobDeclaration::getModuleName() const {
    return moduleName;
}
