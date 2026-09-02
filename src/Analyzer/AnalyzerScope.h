#ifndef ANALYZER_SCOPE_H
#define ANALYZER_SCOPE_H

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>
#include "AnalyzerScopeBlob.h"
#include "AnalyzerScopeBoxed.h"
#include "AnalyzerScopeEnum.h"
#include "AnalyzerScopeProto.h"

class SymbolName;
class ValueType;

using namespace std;

enum class AnalyzerScopeState {
    NOT_REGISTERED,
    DECLARED,
    DEFINED
};

enum class AnalyzerScopeRegisterResult {
    SUCCES,
    FAILURE_ALREADY_DECLARED,
    FAILURE_ALREAD_DEFINED
};

class AnalyzerScope {
friend class AnalyzerScopeBlob;
friend class AnalyzerScopeBoxed;
friend class AnalyzerScopeEnum;
friend class AnalyzerScopeProto;

public:
    typedef struct {
        map<string, shared_ptr<ValueType>> variableTypes;
        map<string, bool> isVariableDefinedMap;

        map<string, shared_ptr<ValueType>> functionTypeMap;
        map<string, bool> isFunctionDefinedMap;

        AnalyzerScopeBlob::ScopeLevel scopeLevelBlob;
        AnalyzerScopeBoxed::ScopeLevel scopeLevelBoxed;
        AnalyzerScopeEnum::ScopeLevel scopeLevelEnum;
        AnalyzerScopeProto::ScopeLevel scopeLevelProto;
    } ScopeLevel;

    AnalyzerScope();

    void pushLevel();
    void popLevel();
    void level(function<void()> levelBlock);

    shared_ptr<ValueType> getVariableType(const string &identifier) const;
    bool setVariableType(const string &identifier, shared_ptr<ValueType> type, bool isDefinition);

    shared_ptr<ValueType> getFunctionType(const string &name) const;
    bool setFunctionType(const string &name, shared_ptr<ValueType> type, bool isDefinition);

    shared_ptr<AnalyzerScopeBlob> blobScope;
    shared_ptr<AnalyzerScopeBoxed> boxedScope;
    shared_ptr<AnalyzerScopeEnum> enumScope;
    shared_ptr<AnalyzerScopeProto> protoScope;

private:
    stack<ScopeLevel> scopeLevels;
};

#endif