#include "AnalyzerScopeProto.h"

#include "AnalyzerScope.h"
#include "Parser/SymbolName.h"
#include "Parser/ValueType/ValueType.h"

using namespace std;

// MARK: - Public

AnalyzerScopeProto::AnalyzerScopeProto(AnalyzerScope *parent):
parent(parent) { }

void AnalyzerScopeProto::registerDeclaration(shared_ptr<SymbolName> symbolName) {
    if (getState(symbolName) != AnalyzerScopeState::NOT_REGISTERED)
        return;

    parent->scopeLevels.back().scopeLevelProto.statesMap[*symbolName] = AnalyzerScopeState::DECLARED;
}

AnalyzerScopeState AnalyzerScopeProto::getState(shared_ptr<SymbolName> symbolName) {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelProto.statesMap.find(*symbolName);
        if (it != (*sit).scopeLevelProto.statesMap.end())
            return (*sit).scopeLevelProto.statesMap[*symbolName];
    }

    return AnalyzerScopeState::NOT_REGISTERED;
}

void AnalyzerScopeProto::registerFields(shared_ptr<SymbolName> symbolName, const vector<pair<string, shared_ptr<ValueType>>> &fields) {
    parent->scopeLevels.back().scopeLevelProto.fieldsMap[*symbolName] = fields;
}

optional<vector<pair<string, shared_ptr<ValueType>>>> AnalyzerScopeProto::getFields(shared_ptr<SymbolName> symbolName) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelProto.fieldsMap.find(*symbolName);
        if (it != (*sit).scopeLevelProto.fieldsMap.end()) {
            vector<pair<string, shared_ptr<ValueType>>> fields = it->second;

            vector<pair<string, shared_ptr<ValueType>>> clonedFields;
            for (pair<string, shared_ptr<ValueType>> &field : fields) {
                clonedFields.push_back({field.first, field.second->clone()});
            }
            return clonedFields;
        }
    }

    return {};
}