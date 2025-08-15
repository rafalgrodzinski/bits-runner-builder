#ifndef STATEMENT_FUNCTION_H
#define STATEMENT_FUNCTION_H

#include "Parser/Statement/Statement.h"

class StatementBlock;
class ValueType;

class StatementFunction: public Statement {
private:
    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunction(string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType, shared_ptr<StatementBlock> statementBlock);
    string getName();
    vector<pair<string, shared_ptr<ValueType>>> getArguments();
    shared_ptr<ValueType> getReturnValueType();
    shared_ptr<StatementBlock> getStatementBlock();
};

#endif