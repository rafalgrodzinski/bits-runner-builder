#include "StatementProtoDeclaration.h"

StatementProtoDeclaration::StatementProtoDeclaration(bool shouldExport, const string &name, shared_ptr<Location> location):
Statement(StatementKind::PROTO_DECLARATION, location), shouldExport(shouldExport), name(std::move(name)) { }

bool StatementProtoDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementProtoDeclaration::getName() const {
    return name;
}