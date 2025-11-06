#include "TypesAnalyzer.h"

#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementFunction.h"

void TypesAnalyzer::checkModule(shared_ptr<StatementModule> module) {
    for (shared_ptr<Statement> statement : module->getStatements())
        checkStatement(statement);
}

void TypesAnalyzer::checkStatement(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::MODULE:
            break;
        case StatementKind::EXPRESSION:
            break;
        case StatementKind::FUNCTION:
            checkStatement(dynamic_pointer_cast<StatementFunction>(statement));
            break;
    }
}

void TypesAnalyzer::checkStatementFunction(shared_ptr<StatementFunction> statementFunction) {
    for (shared_ptr<Statement> statement : statementFunction->getStatementBlock())
        checkStatement(statement);
}