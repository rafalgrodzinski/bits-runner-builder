#include "StatementBlobDeclaration.h"

#include "Parser/SymbolName.h"

using namespace std;

// MARK: - Public

StatementBlobDeclaration::StatementBlobDeclaration(
    bool isVisibleOutside,
    shared_ptr<SymbolName> symbolName,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB_DECLARATION, location),
isVisibleOutside(isVisibleOutside),
symbolName(symbolName) { }

bool StatementBlobDeclaration::getIsVisibleOutside() const {
    return isVisibleOutside;
}

shared_ptr<SymbolName> StatementBlobDeclaration::getSymbolName() const {
    return symbolName;
}

shared_ptr<SymbolName> StatementBlobDeclaration::getPackedSymbolName() const {
    string packedName = format("{}_packed", symbolName->getName());
    return make_shared<SymbolName>(packedName, symbolName->getModuleName());
}