#include "StatementBlob.h"

#include "Parser/ValueType.h"

StatementBlob::StatementBlob(string name, vector<pair<string, shared_ptr<ValueType>>> members, int line, int column):
Statement(StatementKind::BLOB, line, column), name(name), members(members) { }

string StatementBlob::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementBlob::getMembers() {
    return members;
}