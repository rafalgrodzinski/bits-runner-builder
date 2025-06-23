#include "Parser/Statement/Statement.h"

class StatementBlock;

class StatementFunction: public Statement {
private:
    string name;
    vector<pair<string, ValueType>> arguments;
    ValueType returnValueType;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunction(string name, vector<pair<string, ValueType>> arguments, ValueType returnValueType, shared_ptr<StatementBlock> statementBlock);
    string getName();
    vector<pair<string, ValueType>> getArguments();
    ValueType getReturnValueType();
    shared_ptr<StatementBlock> getStatementBlock();
    string toString(int indent) override;
};