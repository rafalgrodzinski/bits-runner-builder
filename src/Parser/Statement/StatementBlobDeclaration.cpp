#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(bool shouldExport, string name, int line, int column):
Statement(StatementKind::BLOB_DECLARATION, line, column), shouldExport(shouldExport), name(name) { }

bool StatementBlobDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementBlobDeclaration::getName() {
    return name;
}