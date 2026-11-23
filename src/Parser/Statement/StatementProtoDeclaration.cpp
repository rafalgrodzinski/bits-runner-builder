#include "StatementProtoDeclaration.h"

#include "Parser/SymbolName.h"

using namespace std;

// MARK: - Public

StatementProtoDeclaration::StatementProtoDeclaration(
    bool isVisibleOutside,
    shared_ptr<SymbolName> symbolName,
    shared_ptr<Location> location
):
Statement(StatementKind::PROTO_DECLARATION, location),
isVisibleOutside(isVisibleOutside),
symbolName(symbolName) { }

bool StatementProtoDeclaration::getIsVisibleOutside() const {
    return isVisibleOutside;
}

shared_ptr<SymbolName> StatementProtoDeclaration::getSymbolName() const {
    return symbolName;
}