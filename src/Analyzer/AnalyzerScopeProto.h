#ifndef ANALYZER_SCOPE_PROTO_H
#define ANALYZER_SCOPE_PROTO_H

#include <map>
#include <optional>
#include <vector>
#include "Parser/SymbolName.h"

class AnalyzerScope;
class ValueType;
enum class AnalyzerScopeState;

class AnalyzerScopeProto {
public:
    typedef struct {
        std::map<SymbolName, AnalyzerScopeState> statesMap;
        std::map<SymbolName, std::vector<std::pair<std::string, std::shared_ptr<ValueType>>>> fieldsMap;
    } ScopeLevel;

    AnalyzerScopeProto(AnalyzerScope *parent);

    void registerDeclaration(std::shared_ptr<SymbolName> symbolName);
    AnalyzerScopeState getState(std::shared_ptr<SymbolName> symbolName);

    void registerFields(std::shared_ptr<SymbolName> symbolName, const std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &fields);
    std::optional<std::vector<std::pair<std::string, std::shared_ptr<ValueType>>>> getFields(std::shared_ptr<SymbolName> symbolName) const;

private:
    AnalyzerScope *parent;
};

#endif