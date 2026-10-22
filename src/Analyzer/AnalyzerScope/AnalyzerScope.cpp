#include "AnalyzerScope.h"

#include "Parser/ValueType/ValueType.h"
#include "Parser/SymbolName.h"

using namespace std;

// MARK: - Public

AnalyzerScope::AnalyzerScope() {
    blobScope = make_shared<AnalyzerScopeBlob>(this);
    boxedScope = make_shared<AnalyzerScopeBoxed>(this);
    enumScope = make_shared<AnalyzerScopeEnum>(this);
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

shared_ptr<ValueType> AnalyzerScope::getFunctionType(const string &name) const {
    stack<ScopeLevel> scopeLevels = this->scopeLevels;

    while (!scopeLevels.empty()) {
        shared_ptr<ValueType> type = scopeLevels.top().functionTypeMap[name];
        if (type != nullptr)
            return type;
        scopeLevels.pop();
    }

    return nullptr;
}

bool AnalyzerScope::setFunctionType(const string &name, shared_ptr<ValueType> type, bool isDefinition) {
    shared_ptr<ValueType> existingType = scopeLevels.top().functionTypeMap[name];
    if (existingType != nullptr) {
        // defining already defined function
        if (scopeLevels.top().isFunctionDefinedMap[name] && isDefinition)
            return false;
        // type doesn't match existing type
        /*if (!existingType->isEqual(type))
            return false;*/
    }

    scopeLevels.top().functionTypeMap[name] = type;
    if (isDefinition)
        scopeLevels.top().isFunctionDefinedMap[name] = true;

    return true;
}