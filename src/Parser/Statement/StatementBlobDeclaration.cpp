#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(bool shouldExport, const string &name, shared_ptr<Location> location):
Statement(StatementKind::BLOB_DECLARATION, location), shouldExport(shouldExport), name(std::move(name)) { }

bool StatementBlobDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementBlobDeclaration::getName() const {
    return name;
}