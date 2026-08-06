#include "StatementBlobDeclaration.h"
#include "Parser/SymbolName.h"

StatementBlobDeclaration::StatementBlobDeclaration(
    bool shouldExport,
    shared_ptr<SymbolName> symbolName,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB_DECLARATION, location),
shouldExport(shouldExport),
symbolName(symbolName) { }

bool StatementBlobDeclaration::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementBlobDeclaration::getSymbolName() const {
    return symbolName;
}
