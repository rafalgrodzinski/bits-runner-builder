#include "Parser/Statement/Statement.h"

class ValueType;

class StatementRawFunction: public Statement {
private:
    string name;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;
    string rawSource;

public:
    StatementRawFunction(
        string name,
        string constraints,
        vector<pair<string, shared_ptr<ValueType>>> arguments,
        shared_ptr<ValueType> returnValueType,
        string rawSource,
        int line,
        int column
    );
    string getName();
    string getConstraints();
    vector<pair<string, shared_ptr<ValueType>>> getArguments();
    shared_ptr<ValueType> getReturnValueType();
    shared_ptr<ValueType> getValueType();
    string getRawSource();
};