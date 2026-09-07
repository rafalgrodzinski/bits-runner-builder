#ifndef ANALYZER_SCOPE_BOXED_H
#define ANALYZER_SCOPE_BOXED_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>
#include "Parser/SymbolName.h"

class AnalyzerScope;
class ValueType;

using namespace std;

class AnalyzerScopeBoxed {
public:
    typedef struct {
        map<string, shared_ptr<ValueType>> namedValueTypesMap;
    } ScopeLevel;

    AnalyzerScopeBoxed(AnalyzerScope *parent);

    shared_ptr<ValueType> getNamedValueType(const string &namedValueTypeKey) const;
    void registerNamedValueTypesMap(const vector<string> &namedValueTypeKeys, const vector<shared_ptr<ValueType>> &namedValueTypes);

private:
    AnalyzerScope *parent;
};

#endif