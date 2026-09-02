#ifndef ANALYZER_SCOPE_PROTO_H
#define ANALYZER_SCOPE_PROTO_H

#include <map>
#include <vector>
#include "Parser/SymbolName.h"

class AnalyzerScope;
class ValueType;
enum class AnalyzerScopeState;

class AnalyzerScopeProto {
public:
    typedef struct {
        map<SymbolName, AnalyzerScopeState> statesMap;
        map<SymbolName, vector<pair<string, shared_ptr<ValueType>>>> fieldsMap;
    } ScopeLevel;

    AnalyzerScopeProto(AnalyzerScope *parent);

    void registerDeclaration(shared_ptr<SymbolName> symbolName);
    AnalyzerScopeState getState(shared_ptr<SymbolName> symbolName);

    void registerFields(shared_ptr<SymbolName> symbolName, const vector<pair<string, shared_ptr<ValueType>>> &fields);
    optional<vector<pair<string, shared_ptr<ValueType>>>> getFields(shared_ptr<SymbolName> symbolName) const;

private:
    AnalyzerScope *parent;
};

#endif