#include "Parser/Statement/Statement.h"

class StatementMetaExternFunction: public Statement {
private:
    string name;
    vector<pair<string, ValueType>> arguments;
    ValueType returnValueType;

public:
    StatementMetaExternFunction(string name, vector<pair<string, ValueType>> arguments, ValueType returnValueType);
    string getName();
    vector<pair<string, ValueType>> getArguments();
    ValueType getReturnValueType();
};