#include "StatementBlob.h"

#include "Parser/ValueType.h"

StatementBlob::StatementBlob(string identifier, vector<pair<string, shared_ptr<ValueType>>> variables):
Statement(StatementKind::BLOB), identifier(identifier), variables(variables) { }

string StatementBlob::getIdentifier() {
    return identifier;
}

vector<pair<string, shared_ptr<ValueType>>> StatementBlob::getVariables() {
    return variables;
}