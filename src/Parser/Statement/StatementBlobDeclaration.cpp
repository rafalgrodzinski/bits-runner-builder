#include "StatementBlobDeclaration.h"

StatementBlobDeclaration::StatementBlobDeclaration(string identifier):
Statement(StatementKind::BLOB_DECLARATION), identifier(identifier) { }

string StatementBlobDeclaration::getIdentifier() {
    return identifier;
}