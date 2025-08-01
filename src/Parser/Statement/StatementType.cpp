#include "StatementType.h"

#include "Parser/ValueType.h"

StatementType::StatementType(string identifier, vector<pair<string, shared_ptr<ValueType>>> variables):
Statement(StatementKind::TYPE), identifier(identifier), variables(variables) { }

string StatementType::getIdentifier() {
    return identifier;
}

vector<pair<string, shared_ptr<ValueType>>> StatementType::getVariables() {
    return variables;
}