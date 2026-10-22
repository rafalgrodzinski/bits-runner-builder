#ifndef ANALYZER_SCOPE_ENUM_H
#define ANALYZER_SCOPE_ENUM_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>

class AnalyzerScope;
class SymbolName;
class ValueType;

enum class AnalyzerScopeRegisterResult;

class AnalyzerScopeEnum {
public:
    typedef struct {
        std::map<SymbolName, std::optional<std::vector<std::string>>> namedValueTypeKeysMap;
        std::map<SymbolName, std::shared_ptr<ValueType>> payloadValueTypesMap;
    } ScopeLevel;

    AnalyzerScopeEnum(AnalyzerScope *parent);

    std::optional<std::vector<std::string>> getNamedValueTypeKeys(std::shared_ptr<SymbolName> symbolName);
    AnalyzerScopeRegisterResult registerNamedValueTypeKeys(std::shared_ptr<SymbolName> symbolName, const std::vector<std::string> &namedValueTypeKeys);

    std::shared_ptr<ValueType> getPayloadValueType(std::shared_ptr<SymbolName> symbolName);
    AnalyzerScopeRegisterResult registerPayloadValueType(std::shared_ptr<SymbolName> symbolName, std::shared_ptr<ValueType> payloadValueType);

private:
    AnalyzerScope *parent;
};

#endif