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

enum class AnalyzerScopeRegisterResult;
enum class AnalyzerScopeState;

class AnalyzerScopeBlob {
public:
    struct Field {
        bool isPublic;
        std::string name;
        std::shared_ptr<ValueType> valueType;
    };

    typedef struct {
        std::optional<std::string> contextBlobName;
        std::map<SymbolName, AnalyzerScopeState> statesMap;
        std::map<SymbolName, std::optional<std::vector<std::string>>> namedValueTypeKeysMap;
        std::map<SymbolName, std::optional<std::vector<Field>>> fieldsMap;
        std::map<SymbolName, std::vector<std::shared_ptr<SymbolName>>> conformingProtoSymbolNamsMap;
    } ScopeLevel;

    AnalyzerScopeBlob(AnalyzerScope *parent);

    AnalyzerScopeRegisterResult registerContextBlobName(const std::string &name);
    std::optional<std::string> getContextBlobName() const;

    AnalyzerScopeRegisterResult registerDeclaration(std::shared_ptr<SymbolName> symbolName);
    AnalyzerScopeState getState(std::shared_ptr<SymbolName> symbolName);

    AnalyzerScopeRegisterResult registerNamedValueTypeKeys(std::shared_ptr<SymbolName> symbolName, const std::vector<std::string> &namedValueTypeKeys);
    std::optional<std::vector<std::string>> getNamedValueTypeKeys(std::shared_ptr<SymbolName> symbolName) const;

    AnalyzerScopeRegisterResult registerFields(std::shared_ptr<SymbolName> symbolName, std::vector<Field> &fields);
    std::optional<std::vector<Field>> getFields(std::shared_ptr<SymbolName> symbolName) const;
    std::optional<std::vector<std::shared_ptr<ValueType>>> getVariableFieldValueTypes(std::shared_ptr<SymbolName> symbolName) const;

    AnalyzerScopeRegisterResult registerConformingProtoSymbolNames(std::shared_ptr<SymbolName> symbolName, std::vector<std::shared_ptr<SymbolName>> conformingProtoSymbolNames);
    std::optional<std::vector<std::shared_ptr<SymbolName>>> getConformingProtoSymbolNames(std::shared_ptr<SymbolName> symbolName);

private:
    AnalyzerScope *parent;
};

#endif