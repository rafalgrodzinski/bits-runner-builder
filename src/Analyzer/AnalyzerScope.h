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
    } ScopeLevel;

    stack<ScopeLevel> scopeLevels;

public:
    AnalyzerScope();

    void pushLevel();
    void popLevel();

    shared_ptr<ValueType> getVariableType(string identifier);
    bool setVariableType(string identifier, shared_ptr<ValueType> type);
};

#endif