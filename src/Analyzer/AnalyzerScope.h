#ifndef ANALYZER_SCOPE_H
#define ANALYZER_SCOPE_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>
#include "AnalyzerScopeBlob.h"
#include "AnalyzerScopeBoxed.h"
#include "AnalyzerScopeEnum.h"

class SymbolName;
class ValueType;

using namespace std;

enum class AnalyzerScopeRegisterResult {
    SUCCES,
    FAILURE_ALREADY_DECLARED,
    FAILURE_ALREAD_DEFINED
};

class AnalyzerScope {
friend class AnalyzerScopeBlob;
friend class AnalyzerScopeBoxed;
friend class AnalyzerScopeEnum;

public:
    typedef struct {
        map<string, optional<vector<pair<string, shared_ptr<ValueType>>>>> protoMembersMap;
        map<string, vector<string>> blobProtosMap;

        vector<string> namedTypes;

        map<string, shared_ptr<ValueType>> variableTypes;
        map<string, bool> isVariableDefinedMap;

        map<string, shared_ptr<ValueType>> functionTypeMap;
        map<string, bool> isFunctionDefinedMap;

        AnalyzerScopeBlob::ScopeLevel scopeLevelBlob;
        AnalyzerScopeBoxed::ScopeLevel scopeLevelBoxed;
        AnalyzerScopeEnum::ScopeLevel scopeLevelEnum;
    } ScopeLevel;

    AnalyzerScope();

    void pushLevel();
    void popLevel();

    optional<vector<pair<string, shared_ptr<ValueType>>>> getProtoMembers(const string &name) const;
    bool setProtoMembers(const string &name, const optional<vector<pair<string, shared_ptr<ValueType>>>> &members);

    bool isNamedTypeDeclared(const string &namedType) const;
    bool setNamedTypes(const vector<string> &namedTypes);

    optional<vector<string>> getBlobProtoNames(const string &name) const;
    bool setBlobProtoNames(const string &name, const vector<string> &protoNames);

    shared_ptr<ValueType> getVariableType(const string &identifier) const;
    bool setVariableType(const string &identifier, shared_ptr<ValueType> type, bool isDefinition);

    shared_ptr<ValueType> getFunctionType(const string &name) const;
    bool setFunctionType(const string &name, shared_ptr<ValueType> type, bool isDefinition);

    shared_ptr<AnalyzerScopeBlob> blobScope;
    shared_ptr<AnalyzerScopeBoxed> boxedScope;
    shared_ptr<AnalyzerScopeEnum> enumScope;

private:
    stack<ScopeLevel> scopeLevels;
};

#endif