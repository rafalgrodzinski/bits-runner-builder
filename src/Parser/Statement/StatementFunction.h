#ifndef STATEMENT_FUNCTION_H
#define STATEMENT_FUNCTION_H

#include "Parser/Statement/Statement.h"

class StatementBlock;
class ValueType;

class StatementFunction: public Statement {
friend class Parser;

private:
    bool shouldExport;
    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunction(
        bool shouldExport,
        string name,
        vector<pair<string, shared_ptr<ValueType>>> arguments,
        shared_ptr<ValueType> returnValueType,
        shared_ptr<StatementBlock> statementBlock,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getName() const;
    vector<pair<string, shared_ptr<ValueType>>> getArguments() const;
    shared_ptr<ValueType> getReturnValueType() const;
    shared_ptr<ValueType> getValueType() const;
    shared_ptr<StatementBlock> getStatementBlock() const;
};

#endif