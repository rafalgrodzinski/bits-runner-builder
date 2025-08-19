#include "StatementFunction.h"

#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementReturn.h"

StatementFunction::StatementFunction(bool shouldExport, string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType, shared_ptr<StatementBlock> statementBlock):
Statement(StatementKind::FUNCTION), shouldExport(shouldExport), name(name), arguments(arguments), returnValueType(returnValueType), statementBlock(statementBlock) {
    vector<shared_ptr<Statement>> statements = statementBlock->getStatements();
    if (!statements.empty() && statements.back()->getKind() == StatementKind::RETURN)
        return;

    // add an emty return statement if none is present
    shared_ptr<StatementReturn> statementReturn = make_shared<StatementReturn>(nullptr);
    statements.push_back(statementReturn);
    this->statementBlock = make_shared<StatementBlock>(statements);
}

bool StatementFunction::getShouldExport() {
    return shouldExport;
}

string StatementFunction::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementFunction::getArguments() {
    return arguments;
}

shared_ptr<ValueType> StatementFunction::getReturnValueType() {
    return returnValueType;
}

shared_ptr<StatementBlock> StatementFunction::getStatementBlock() {
    return statementBlock;
}
