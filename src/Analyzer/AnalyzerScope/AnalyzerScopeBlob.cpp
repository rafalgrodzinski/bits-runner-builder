#include "AnalyzerScopeBlob.h"

#include "AnalyzerScope.h"
#include "Parser/ValueType/ValueType.h"

using namespace std;

// MARK: - Public

AnalyzerScopeBlob::AnalyzerScopeBlob(AnalyzerScope *parent):
parent(parent) { }

AnalyzerScopeRegisterResult AnalyzerScopeBlob::registerContextBlobName(const std::string &name) {
    parent->scopeLevels.back().scopeLevelBlob.contextBlobName = name;
    return AnalyzerScopeRegisterResult::SUCCES;
}

optional<string> AnalyzerScopeBlob::getContextBlobName() const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        if ((*sit).scopeLevelBlob.contextBlobName)
            return (*sit).scopeLevelBlob.contextBlobName;
    }

    return "";
}

AnalyzerScopeRegisterResult AnalyzerScopeBlob::registerDeclaration(shared_ptr<SymbolName> symbolName) {
    if (getState(symbolName) != AnalyzerScopeState::NOT_REGISTERED)
        return AnalyzerScopeRegisterResult::SUCCES;

    parent->scopeLevels.back().scopeLevelBlob.statesMap[*symbolName] = AnalyzerScopeState::DECLARED;
    return AnalyzerScopeRegisterResult::SUCCES;
}

AnalyzerScopeState AnalyzerScopeBlob::getState(shared_ptr<SymbolName> symbolName) {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.statesMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.statesMap.end())
            return (*sit).scopeLevelBlob.statesMap[*symbolName];
    }

    return AnalyzerScopeState::NOT_REGISTERED;
}

AnalyzerScopeRegisterResult AnalyzerScopeBlob::registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys) {
    // check if named types are already defined
    if (parent->scopeLevels.back().scopeLevelBlob.namedValueTypeKeysMap.find(*symbolName) != parent->scopeLevels.back().scopeLevelBlob.namedValueTypeKeysMap.end())
        return AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED;

    parent->scopeLevels.back().scopeLevelBlob.namedValueTypeKeysMap[*symbolName] = namedValueTypeKeys;
    return AnalyzerScopeRegisterResult::SUCCES;
}

optional<vector<string>> AnalyzerScopeBlob::getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.namedValueTypeKeysMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.namedValueTypeKeysMap.end())
            return (*sit).scopeLevelBlob.namedValueTypeKeysMap[*symbolName];
    }

    return {};
}

AnalyzerScopeRegisterResult AnalyzerScopeBlob::registerFields(shared_ptr<SymbolName> symbolName, vector<Field> &fields) {
    if (parent->scopeLevels.back().scopeLevelBlob.fieldsMap.find(*symbolName) != parent->scopeLevels.back().scopeLevelBlob.fieldsMap.end())
        return AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED;

    parent->scopeLevels.back().scopeLevelBlob.fieldsMap[*symbolName] = fields;
    return AnalyzerScopeRegisterResult::SUCCES;
}

optional<vector<AnalyzerScopeBlob::Field>> AnalyzerScopeBlob::getFields(shared_ptr<SymbolName> symbolName) const {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.fieldsMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.fieldsMap.end()) {
            vector<Field> &fields = *it->second;

            vector<Field> clonedFields;
            for (Field &field : fields) {
                clonedFields.emplace_back(field.isPublic, field.name, field.valueType->clone());
            }
            return clonedFields;
        }
    }

    return {};
}

optional<vector<shared_ptr<ValueType>>> AnalyzerScopeBlob::getVariableFieldValueTypes(std::shared_ptr<SymbolName> symbolName) const {
    optional<vector<Field>> fields = getFields(symbolName);
    if (!fields)
        return { };

    vector<shared_ptr<ValueType>> valueTypes;
    for (const Field &field : *fields) {
        if (!field.valueType->isFun())
            valueTypes.push_back(field.valueType);
    }

    return valueTypes;
}

AnalyzerScopeRegisterResult AnalyzerScopeBlob::registerConformingProtoSymbolNames(shared_ptr<SymbolName> symbolName, vector<shared_ptr<SymbolName>> conformingProtoSymbolNames) {
    if (parent->scopeLevels.back().scopeLevelBlob.conformingProtoSymbolNamsMap.find(*symbolName) != parent->scopeLevels.back().scopeLevelBlob.conformingProtoSymbolNamsMap.end())
        return AnalyzerScopeRegisterResult::FAILURE_ALREAD_DEFINED;

    parent->scopeLevels.back().scopeLevelBlob.conformingProtoSymbolNamsMap[*symbolName] = conformingProtoSymbolNames;
    return AnalyzerScopeRegisterResult::SUCCES;
}

optional<vector<shared_ptr<SymbolName>>> AnalyzerScopeBlob::getConformingProtoSymbolNames(shared_ptr<SymbolName> symbolName) {
    vector<AnalyzerScope::ScopeLevel> &scopeLevels = parent->scopeLevels;

    for (auto sit = scopeLevels.rbegin(); sit != scopeLevels.rend(); sit++) {
        auto it = (*sit).scopeLevelBlob.conformingProtoSymbolNamsMap.find(*symbolName);
        if (it != (*sit).scopeLevelBlob.conformingProtoSymbolNamsMap.end())
            return (*sit).scopeLevelBlob.conformingProtoSymbolNamsMap[*symbolName];
    }

    return {};
}