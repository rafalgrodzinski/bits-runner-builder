#ifndef ANALYZER_SCOPE_BLOB_H
#define ANALYZER_SCOPE_BLOB_H

#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "Parser/SymbolName.h"

class AnalyzerScope;
class ValueType;

enum class AnalyzerScopeState;

class AnalyzerScopeBlob {
public:
    typedef struct {
        std::map<SymbolName, AnalyzerScopeState> statesMap;
        std::map<SymbolName, std::optional<std::vector<std::string>>> namedValueTypeKeysMap;
        std::map<SymbolName, std::optional<std::vector<std::pair<std::string, std::shared_ptr<ValueType>>>>> fieldsMap;
        std::map<SymbolName, std::vector<std::shared_ptr<SymbolName>>> conformingProtoSymbolNamsMap;
    } ScopeLevel;

    AnalyzerScopeBlob(AnalyzerScope *parent);

    void registerDeclaration(std::shared_ptr<SymbolName> symbolName);
    AnalyzerScopeState getState(std::shared_ptr<SymbolName> symbolName);

    void registerNamedValueTypeKeys(std::shared_ptr<SymbolName> symbolName, const std::vector<std::string> &namedValueTypeKeys);
    std::optional<std::vector<std::string>> getNamedValueTypeKeys(std::shared_ptr<SymbolName> symbolName) const;

    void registerFields(std::shared_ptr<SymbolName> symbolName, std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> &fields);
    std::optional<std::vector<std::pair<std::string, std::shared_ptr<ValueType>>>> getFields(std::shared_ptr<SymbolName> symbolName) const;
    std::optional<std::vector<std::shared_ptr<ValueType>>> getVariableFieldValueTypes(std::shared_ptr<SymbolName> symbolName) const;

    void registerConformingProtoSymbolNames(std::shared_ptr<SymbolName> symbolName, std::vector<std::shared_ptr<SymbolName>> conformingProtoSymbolNames);
    std::optional<std::vector<std::shared_ptr<SymbolName>>> getConformingProtoSymbolNames(std::shared_ptr<SymbolName> symbolName);

private:
    AnalyzerScope *parent;
};

#endif