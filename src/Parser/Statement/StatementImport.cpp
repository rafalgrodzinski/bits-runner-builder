#include "StatementImport.h"

StatementImport::StatementImport(string name):
Statement(StatementKind::META_IMPORT), name(name) { }

string StatementImport::getName() {
    return name;
}