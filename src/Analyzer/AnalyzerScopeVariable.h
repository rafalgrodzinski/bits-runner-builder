#ifndef ANALYZER_SCOPE_VARIABLE_H
#define ANALYZER_SCOPE_VARIABLE_H

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

class AnalyzerScope;
class SymbolName;
class ValueType;

class AnalyzerScopeVariable {
public:
    typedef struct {
        int anonymousValueTypesCount;
        std::map<std::string, std::shared_ptr<ValueType>> valueTypesMap;
    } ScopeLevel;

    AnalyzerScopeVariable(AnalyzerScope *parent);

    std::shared_ptr<ValueType> getValueType(const std::string &name) const;
    void registerValueType(const std::string &name, std::shared_ptr<ValueType> valueType);
    void registerAnonymousValueType(std::shared_ptr<ValueType> valueType);

private:
    AnalyzerScope *parent;
};

#endif