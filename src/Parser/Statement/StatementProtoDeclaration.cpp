#include "StatementProtoDeclaration.h"
#include "Parser/SymbolName.h"

StatementProtoDeclaration::StatementProtoDeclaration(
    bool shouldExport,
    shared_ptr<SymbolName> symbolName,
    shared_ptr<Location> location
):
Statement(StatementKind::PROTO_DECLARATION, location),
symbolName(symbolName) { }

bool StatementProtoDeclaration::getShouldExport() const {
    return shouldExport;
}

shared_ptr<SymbolName> StatementProtoDeclaration::getSymbolName() const {
    return symbolName;
}