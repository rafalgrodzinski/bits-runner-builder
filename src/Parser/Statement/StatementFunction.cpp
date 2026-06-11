#include "StatementFunction.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/ValueType/ValueType.h"

StatementFunction::StatementFunction(
    bool shouldExport,
    string name,
    vector<pair<string, shared_ptr<ValueType>>> arguments,
    shared_ptr<ValueType> returnValueType,
    shared_ptr<StatementBlock> statementBlock,
    shared_ptr<Location> location
):
Statement(StatementKind::FUNCTION, location), shouldExport(shouldExport), name(std::move(name)), arguments(std::move(arguments)), returnValueType(returnValueType), statementBlock(statementBlock) {
    vector<shared_ptr<Statement>> statements = statementBlock->getStatements();
    if (!statements.empty() && statements.back()->getKind() == StatementKind::RETURN)
        return;

    // add an empty return statement if none is present
    shared_ptr<StatementReturn> statementReturn = make_shared<StatementReturn>(Expression::NONE, location);
    statements.push_back(statementReturn);
    this->statementBlock = make_shared<StatementBlock>(statements, statementBlock->getLocation());
}

bool StatementFunction::getShouldExport() const {
    return shouldExport;
}

string StatementFunction::getName() const {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementFunction::getArguments() const {
    return arguments;
}

shared_ptr<ValueType> StatementFunction::getReturnValueType() const {
    return returnValueType;
}

shared_ptr<ValueType> StatementFunction::getValueType() const {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}

shared_ptr<StatementBlock> StatementFunction::getStatementBlock() const {
    return statementBlock;
}
