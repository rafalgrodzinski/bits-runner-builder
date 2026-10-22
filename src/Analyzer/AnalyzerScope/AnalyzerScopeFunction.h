#ifndef ANALYZER_SCOPE_FUNCTION_H
#define ANALYZER_SCOPE_FUNCTION_H

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

class AnalyzerScope;
class ValueType;

enum class AnalyzerScopeRegisterResult;
enum class AnalyzerScopeState;

class AnalyzerScopeFunction {
public:
    typedef struct {
        std::map<std::string, AnalyzerScopeState> registerStatesMap;
        std::map<std::string, std::shared_ptr<ValueType>> valueTypesMap;
    } ScopeLevel;

    AnalyzerScopeFunction(AnalyzerScope *parent);

    std::shared_ptr<ValueType> getValueType(const std::string &name) const;
    AnalyzerScopeRegisterResult registerValueType(const std::string &name, std::shared_ptr<ValueType> valueType, bool isDefinition);

private:
    AnalyzerScope *parent;
};

#endif