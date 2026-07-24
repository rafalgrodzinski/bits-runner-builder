#ifndef ANALYZER_SCOPE_H
#define ANALYZER_SCOPE_H

#include <map>
#include <string>
#include <stack>
#include <memory>
#include <vector>
#include <optional>

class SymbolName;
class ValueType;
struct EnumField;

using namespace std;

class AnalyzerScopeEnum {
public:
    void registerNamedValueTypeKeys(shared_ptr<SymbolName> symbolName, const vector<string> &namedValueTypeKeys);
    optional<vector<string>> getNamedValueTypeKeys(shared_ptr<SymbolName> symbolName);
};

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

public:
    AnalyzerScope();

    void pushLevel();
    void popLevel();

    optional<vector<pair<string, shared_ptr<ValueType>>>> getProtoMembers(const string &name) const;
    bool setProtoMembers(const string &name, const optional<vector<pair<string, shared_ptr<ValueType>>>> &members);
    
    optional<vector<pair<string, shared_ptr<ValueType>>>> getBlobMembers(shared_ptr<ValueType> valueType) const;
    optional<vector<shared_ptr<ValueType>>> getNonFunctionBlobMemberTypes(shared_ptr<ValueType> valueType) const;
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

    //bool registerEnumFields(const string &enumGlobalName, const vector<EnumField> &fields);
    shared_ptr<AnalyzerScopeEnum> enums;

private:
    stack<ScopeLevel> scopeLevels;
};

#endif