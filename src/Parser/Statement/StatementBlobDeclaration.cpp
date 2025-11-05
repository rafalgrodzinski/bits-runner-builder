#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(string name):
Statement(StatementKind::BLOB_DECLARATION), name(name) { }

string StatementBlobDeclaration::getName() {
    return name;
}