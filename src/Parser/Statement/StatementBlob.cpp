#include "StatementBlob.h"

#include "Parser/ValueType.h"

StatementBlob::StatementBlob(bool shouldExport, string name, vector<pair<string, shared_ptr<ValueType>>> members, int line, int column):
Statement(StatementKind::BLOB, line, column), shouldExport(shouldExport), name(name), members(members) { }

bool StatementBlob::getShouldExport() {
    return shouldExport;
}

string StatementBlob::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementBlob::getMembers() {
    return members;
}