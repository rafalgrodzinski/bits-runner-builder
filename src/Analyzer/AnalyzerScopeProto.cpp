#include "AnalyzerScopeProto.h"
#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

AnalyzerScopeProto::AnalyzerScopeProto(AnalyzerScope *parent):
parent(parent) { }

void AnalyzerScopeProto::registerDeclaration(shared_ptr<SymbolName> symbolName) {
    if (getState(symbolName) != AnalyzerScopeState::NOT_REGISTERED)
        return;

    parent->scopeLevels.top().scopeLevelProto.statesMap[*symbolName] = AnalyzerScopeState::DECLARED;
}

AnalyzerScopeState AnalyzerScopeProto::getState(shared_ptr<SymbolName> symbolName) {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelProto.statesMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelProto.statesMap.end())
            return scopeLevels.top().scopeLevelProto.statesMap[*symbolName];
        scopeLevels.pop();
    }

    return AnalyzerScopeState::NOT_REGISTERED;
}

void AnalyzerScopeProto::registerFields(shared_ptr<SymbolName> symbolName, const vector<pair<string, shared_ptr<ValueType>>> &fields) {
    parent->scopeLevels.top().scopeLevelProto.fieldsMap[*symbolName] = fields;
}

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScopeProto::getFields(shared_ptr<SymbolName> symbolName) const {
    stack<AnalyzerScope::ScopeLevel> scopeLevels = parent->scopeLevels;

    while (!scopeLevels.empty()) {
        auto it = scopeLevels.top().scopeLevelProto.fieldsMap.find(*symbolName);
        if (it != scopeLevels.top().scopeLevelProto.fieldsMap.end()) {
            vector<pair<string, shared_ptr<ValueType>>> fields = it->second;

            vector<pair<string, shared_ptr<ValueType>>> clonedFields;
            for (pair<string, shared_ptr<ValueType>> &field : fields) {
                clonedFields.push_back({field.first, field.second->clone()});
            }
            return clonedFields;
        }
        scopeLevels.pop();
    }

    return {};
}