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
        map<string, optional<vector<string>>> blobNamedTypeKeysMap;
        map<string, optional<vector<pair<string, shared_ptr<ValueType>>>>> protoMembersMap;
        map<string, vector<string>> blobProtosMap;
        map<string, optional<vector<pair<string, shared_ptr<ValueType>>>>> blobMembersMap;

        vector<string> namedTypes;

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

    optional<vector<pair<string, shared_ptr<ValueType>>>> getProtoMembers(const string &name) const;
    bool setProtoMembers(const string &name, const optional<vector<pair<string, shared_ptr<ValueType>>>> &members);
    
    optional<vector<pair<string, shared_ptr<ValueType>>>> getBlobMembers(shared_ptr<ValueType> blobValueType) const;
    optional<vector<shared_ptr<ValueType>>> getNonFunctionBlobMemberTypes(shared_ptr<ValueType> blobValueType) const;
    bool isBlobDeclared(const string &name) const;
    bool setBlobMembers(const string &name, const optional<vector<pair<string, shared_ptr<ValueType>>>> &members);

    bool isNamedTypeDeclared(const string &namedType) const;
    bool setNamedTypes(const vector<string> &namedTypes);

    optional<vector<string>> getBlobNamedTypeKeys(const string &blobName) const;
    bool setBlobNamedTypeKeys(const string &blobName, const vector<string> &namedTypeKeys);

    optional<vector<string>> getBlobProtoNames(const string &name) const;
    bool setBlobProtoNames(const string &name, const vector<string> &protoNames);

    shared_ptr<ValueType> getVariableType(const string &identifier) const;
    bool setVariableType(const string &identifier, shared_ptr<ValueType> type, bool isDefinition);

    shared_ptr<ValueType> getFunctionType(const string &name) const;
    bool setFunctionType(const string &name, shared_ptr<ValueType> type, bool isDefinition);
};

#endif