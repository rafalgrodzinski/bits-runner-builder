#include "StatementMetaImport.h"

StatementMetaImport::StatementMetaImport(const string &name, shared_ptr<Location> location):
Statement(StatementKind::META_IMPORT, location), name(name) { }

string StatementMetaImport::getName() const {
    return name;
}