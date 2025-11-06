#include "StatementImport.h"

StatementImport::StatementImport(string name, int line, int column):
Statement(StatementKind::META_IMPORT, line, column), name(name) { }

string StatementImport::getName() {
    return name;
}