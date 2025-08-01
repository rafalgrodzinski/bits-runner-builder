#include "StatementType.h"

StatementType::StatementType(string identifier, vector<shared_ptr<StatementVariable>> statementVariable):
Statement(StatementKind::TYPE), identifier(identifier), statementVariables(statementVariable) { }

string StatementType::getIdentifier() {
    return identifier;
}

vector<shared_ptr<StatementVariable>> StatementType::getStatementVariables() {
    return statementVariables;
}