#ifndef ANALYZER_SCOPE_H
#define ANALYZER_SCOPE_H

#include <map>
#include <string>
#include <stack>
#include <memory>
#include <vector>
#include <optional>

class ValueType;

using namespace std;

class AnalyzerScope {
private:
    typedef struct {
        map<string, optional<vector<pair<string, shared_ptr<ValueType>>>>> protoMembersMap;
        map<string, optional<vector<pair<string, shared_ptr<ValueType>>>>> blobMembersMap;

        map<string, shared_ptr<ValueType>> variableTypes;
        map<string, bool> isVariableDefinedMap;

        map<string, shared_ptr<ValueType>> functionTypeMap;
        map<string, bool> isFunctionDefinedMap;
    } ScopeLevel;

    stack<ScopeLevel> scopeLevels;

public:
    AnalyzerScope();

    void pushLevel();
    void popLevel();

    optional<vector<pair<string, shared_ptr<ValueType>>>> getProtoMembers(string name);
    bool setProtoMembers(string name, optional<vector<pair<string, shared_ptr<ValueType>>>> members);
    
    optional<vector<pair<string, shared_ptr<ValueType>>>> getBlobMembers(string name);
    bool setBlobMembers(string name, optional<vector<pair<string, shared_ptr<ValueType>>>> members);

    shared_ptr<ValueType> getVariableType(string identifier);
    bool setVariableType(string identifier, shared_ptr<ValueType> type, bool isDefinition);

    shared_ptr<ValueType> getFunctionType(string name);
    bool setFunctionType(string name, shared_ptr<ValueType> type, bool isDefinition);
};

#endif