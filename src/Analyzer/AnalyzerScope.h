#ifndef ANALYZER_SCOPE_H
#define ANALYZER_SCOPE_H

#include <map>
#include <string>
#include <stack>

class ValueType;

using namespace std;

class AnalyzerScope {
private:
    typedef struct {
        map<string, shared_ptr<ValueType>> variableTypes;
        map<string, vector<shared_ptr<ValueType>>> functionArgumentTypesMap;
        map<string, shared_ptr<ValueType>> functionReturnTypeMap;
    } ScopeLevel;

    stack<ScopeLevel> scopeLevels;

public:
    AnalyzerScope();

    void pushLevel();
    void popLevel();

    shared_ptr<ValueType> getVariableType(string identifier);
    bool setVariableType(string identifier, shared_ptr<ValueType> type);

    optional<vector<shared_ptr<ValueType>>> getFunctionArgumentTypes(string name);
    bool setFunctionArgumentTypes(string name, vector<shared_ptr<ValueType>> argumentTypes);

    shared_ptr<ValueType> getFunctionReturnType(string name);
    bool setFunctionReturnType(string name, shared_ptr<ValueType> returnType);
};

#endif