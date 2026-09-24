#ifndef ANALYZER_SCOPE_BOXED_H
#define ANALYZER_SCOPE_BOXED_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>
#include "Parser/SymbolName.h"

class AnalyzerScope;
class ValueType;

class AnalyzerScopeBoxed {
public:
    typedef struct {
        std::map<std::string, std::shared_ptr<ValueType>> namedValueTypesMap;
    } ScopeLevel;

    AnalyzerScopeBoxed(AnalyzerScope *parent);

    std::shared_ptr<ValueType> getNamedValueType(const std::string &namedValueTypeKey) const;
    void registerNamedValueTypesMap(const std::vector<std::string> &namedValueTypeKeys, const std::vector<std::shared_ptr<ValueType>> &namedValueTypes);

private:
    AnalyzerScope *parent;
};

#endif