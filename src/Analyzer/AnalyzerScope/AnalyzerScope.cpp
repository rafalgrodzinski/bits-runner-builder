#include "AnalyzerScope.h"

#include "Parser/ValueType/ValueType.h"
#include "Parser/SymbolName.h"

using namespace std;

// MARK: - Public

AnalyzerScope::AnalyzerScope() {
    blobScope = make_shared<AnalyzerScopeBlob>(this);
    boxedScope = make_shared<AnalyzerScopeBoxed>(this);
    enumScope = make_shared<AnalyzerScopeEnum>(this);
    functionScope = make_shared<AnalyzerScopeFunction>(this);
    protoScope = make_shared<AnalyzerScopeProto>(this);
    variableScope = make_shared<AnalyzerScopeVariable>(this);

    pushLevel();
}

void AnalyzerScope::pushLevel() {
    scopeLevels.push(ScopeLevel());
}

void AnalyzerScope::popLevel() {
    scopeLevels.pop();
}

bool AnalyzerScope::level(function<bool ()> levelBlock) {
    pushLevel();
    bool isSuccess = levelBlock();
    popLevel();
    return isSuccess;
}