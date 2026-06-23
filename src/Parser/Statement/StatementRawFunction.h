#ifndef STATEMENT_FUNCTION
#define STATEMENT_FUNCTION

#include <format>

#include "Parser/Statement/Statement.h"

class ValueType;

class StatementRawFunction: public Statement {
private:
    bool shouldExport;
    string name;
    string moduleName;
    string constraints;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;
    string rawSource;

public:
    StatementRawFunction(
        bool shouldExport,
        const string &name,
        const string &constraints,
        const vector<pair<string, shared_ptr<ValueType>>> &arguments,
        shared_ptr<ValueType> returnValueType,
        const string &rawSource,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getName() const;
    string getGlobalName() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);
    string getConstraints() const;
    vector<pair<string, shared_ptr<ValueType>>> getArguments() const;
    shared_ptr<ValueType> getReturnValueType() const;
    shared_ptr<ValueType> getValueType() const;
    string getRawSource() const;
};

#endif