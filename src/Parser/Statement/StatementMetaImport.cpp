#include "StatementMetaImport.h"

StatementMetaImport::StatementMetaImport(string name, int line, int column):
Statement(StatementKind::META_IMPORT, line, column), name(name) { }

string StatementMetaImport::getName() {
    return name;
}