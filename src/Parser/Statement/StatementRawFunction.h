#ifndef STATEMENT_FUNCTION
#define STATEMENT_FUNCTION

#include "Parser/Statement/Statement.h"

class ValueType;

class StatementRawFunction: public Statement {
private:
    bool shouldExport;
    string name;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;
    string rawSource;

public:
    StatementRawFunction(
        bool shouldExport,
        string name,
        string constraints,
        vector<pair<string, shared_ptr<ValueType>>> arguments,
        shared_ptr<ValueType> returnValueType,
        string rawSource,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getName() const;
    string getConstraints() const;
    vector<pair<string, shared_ptr<ValueType>>> getArguments() const;
    shared_ptr<ValueType> getReturnValueType() const;
    shared_ptr<ValueType> getValueType() const;
    string getRawSource() const;
};

#endif