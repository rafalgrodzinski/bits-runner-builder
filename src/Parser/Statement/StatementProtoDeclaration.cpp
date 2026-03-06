#include "StatementProtoDeclaration.h"

StatementProtoDeclaration::StatementProtoDeclaration(bool shouldExport, string name, shared_ptr<Location> location):
Statement(StatementKind::PROTO_DECLARATION, location), shouldExport(shouldExport), name(name) { }

bool StatementProtoDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementProtoDeclaration::getName() {
    return name;
}