#include "StatementFunction.h"

#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementReturn.h"

static string valueTypeToString(ValueType valueType) {
    switch (valueType) {
        case ValueType::NONE:
            return "NONE";
        case ValueType::BOOL:
            return "BOOL";
        case ValueType::SINT32:
            return "SINT32";
        case ValueType::REAL32:
            return "REAL32";
    }
}

StatementFunction::StatementFunction(string name, vector<pair<string, ValueType>> arguments, ValueType returnValueType, shared_ptr<StatementBlock> statementBlock):
Statement(StatementKind::FUNCTION), name(name), arguments(arguments), returnValueType(returnValueType), statementBlock(statementBlock) {
    vector<shared_ptr<Statement>> statements = statementBlock->getStatements();
    if (!statements.empty() && statements.back()->getKind() == StatementKind::RETURN)
        return;

    // add an emty return statement if none is present
    shared_ptr<StatementReturn> statementReturn = make_shared<StatementReturn>(nullptr);
    statements.push_back(statementReturn);
    this->statementBlock = make_shared<StatementBlock>(statements);
}

string StatementFunction::getName() {
    return name;
}

vector<pair<string, ValueType>> StatementFunction::getArguments() {
    return arguments;
}

ValueType StatementFunction::getReturnValueType() {
    return returnValueType;
}

shared_ptr<StatementBlock> StatementFunction::getStatementBlock() {
    return statementBlock;
}
