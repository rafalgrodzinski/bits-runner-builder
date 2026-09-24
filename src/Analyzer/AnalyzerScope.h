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
        std::map<std::string, std::shared_ptr<ValueType>> variableTypes;
        std::map<std::string, bool> isVariableDefinedMap;

        std::map<std::string, std::shared_ptr<ValueType>> functionTypeMap;
        std::map<std::string, bool> isFunctionDefinedMap;

        AnalyzerScopeBlob::ScopeLevel scopeLevelBlob;
        AnalyzerScopeBoxed::ScopeLevel scopeLevelBoxed;
        AnalyzerScopeEnum::ScopeLevel scopeLevelEnum;
        AnalyzerScopeProto::ScopeLevel scopeLevelProto;
    } ScopeLevel;

    AnalyzerScope();

    void pushLevel();
    void popLevel();
    bool level(std::function<bool ()> levelBlock);

    std::shared_ptr<ValueType> getVariableType(const std::string &identifier) const;
    bool setVariableType(const std::string &identifier, std::shared_ptr<ValueType> type, bool isDefinition);

    std::shared_ptr<ValueType> getFunctionType(const std::string &name) const;
    bool setFunctionType(const std::string &name, std::shared_ptr<ValueType> type, bool isDefinition);

    std::shared_ptr<AnalyzerScopeBlob> blobScope;
    std::shared_ptr<AnalyzerScopeBoxed> boxedScope;
    std::shared_ptr<AnalyzerScopeEnum> enumScope;
    std::shared_ptr<AnalyzerScopeProto> protoScope;

private:
    std::stack<ScopeLevel> scopeLevels;
};

#endif