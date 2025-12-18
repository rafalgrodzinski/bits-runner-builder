#include "StatementMetaImport.h"

StatementMetaImport::StatementMetaImport(string name, shared_ptr<Location> location):
Statement(StatementKind::META_IMPORT, location), name(name) { }

string StatementMetaImport::getName() {
    return name;
}