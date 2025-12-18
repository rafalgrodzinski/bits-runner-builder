#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(bool shouldExport, string name, shared_ptr<Location> location):
Statement(StatementKind::BLOB_DECLARATION, location), shouldExport(shouldExport), name(name) { }

bool StatementBlobDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementBlobDeclaration::getName() {
    return name;
}