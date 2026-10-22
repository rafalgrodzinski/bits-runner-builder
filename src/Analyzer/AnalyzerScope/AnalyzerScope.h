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
#include "AnalyzerScopeFunction.h"
#include "AnalyzerScopeProto.h"
#include "AnalyzerScopeVariable.h"

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
friend class AnalyzerScopeFunction;
friend class AnalyzerScopeProto;
friend class AnalyzerScopeVariable;

public:
    typedef struct {
        AnalyzerScopeBlob::ScopeLevel scopeLevelBlob;
        AnalyzerScopeBoxed::ScopeLevel scopeLevelBoxed;
        AnalyzerScopeEnum::ScopeLevel scopeLevelEnum;
        AnalyzerScopeFunction::ScopeLevel scopeLevelFunction;
        AnalyzerScopeProto::ScopeLevel scopeLevelProto;
        AnalyzerScopeVariable::ScopeLevel scopeLevelVariable;
    } ScopeLevel;

    AnalyzerScope();

    void pushLevel();
    void popLevel();
    bool level(std::function<bool ()> levelBlock);

    std::shared_ptr<AnalyzerScopeBlob> blobScope;
    std::shared_ptr<AnalyzerScopeBoxed> boxedScope;
    std::shared_ptr<AnalyzerScopeEnum> enumScope;
    std::shared_ptr<AnalyzerScopeFunction> functionScope;
    std::shared_ptr<AnalyzerScopeProto> protoScope;
    std::shared_ptr<AnalyzerScopeVariable> variableScope;

private:
    std::stack<ScopeLevel> scopeLevels;
};

#endif