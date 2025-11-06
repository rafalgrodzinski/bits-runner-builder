#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(string name, int line, int column):
Statement(StatementKind::BLOB_DECLARATION, line, column), name(name) { }

string StatementBlobDeclaration::getName() {
    return name;
}