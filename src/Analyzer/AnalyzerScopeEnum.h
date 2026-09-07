#ifndef ANALYZER_SCOPE_ENUM_H
#define ANALYZER_SCOPE_ENUM_H

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

class AnalyzerScopeEnum {
public:
    typedef struct {
        map<SymbolName, optional<vector<string>>> namedValueTypeKeys;
        map<SymbolName, shared_ptr<ValueType>> payloadValueType;
    } ScopeLevel;

    AnalyzerScopeEnum(AnalyzerScope *parent);

    optional<vector<string>> getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName);
    void registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys);

    shared_ptr<ValueType> getPayloadValueType(shared_ptr<SymbolName> symbolName);
    void registerPayloadValueType(shared_ptr<SymbolName> symbolName, shared_ptr<ValueType> payloadValueType);

private:
    AnalyzerScope *parent;
};

#endif