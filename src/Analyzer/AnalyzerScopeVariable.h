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

enum class AnalyzerScopeRegisterResult;
enum class AnalyzerScopeState;

class AnalyzerScopeVariable {
public:
    typedef struct {
        int anonymousValueTypesCount;
        std::map<std::string, AnalyzerScopeState> statesMap;
        std::map<std::string, std::shared_ptr<ValueType>> valueTypesMap;
    } ScopeLevel;

    AnalyzerScopeVariable(AnalyzerScope *parent);

    std::shared_ptr<ValueType> getValueType(const std::string &identifier) const;
    AnalyzerScopeRegisterResult registerValueType(const std::string &identifier, std::shared_ptr<ValueType> valueType, bool isDefinition);
    AnalyzerScopeRegisterResult registerAnonymousValueType(std::shared_ptr<ValueType> valueType);

private:
    AnalyzerScope *parent;
};

#endif