#ifndef STATEMENT_VARIABLE_DECLARATION_H
#define STATEMENT_VARIABLE_DECLARATION_H

#include <format>

#include "Statement.h"

class ValueType;

enum class VariableKind {
    MODULE,
    BLOB,
    PROTO
};

class StatementVariableDeclaration: public Statement {
friend class Analyzer;

public:
    StatementVariableDeclaration(
        bool shouldExport,
        bool isPublic,
        VariableKind kind,
        const std::string &identifier,
        const std::string &moduleName,
        bool isRoot,
        std::shared_ptr<ValueType> valueType,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    bool getIsPublic() const;
    VariableKind getKind() const;
    std::string getIdentifier() const;
    std::string getGlobalIdentifier() const;
    std::string getModuleName() const;
    void setModuleName(const std::string &moduleName);
    bool getIsRoot() const;
    std::shared_ptr<ValueType> getValueType() const;

private:
    bool shouldExport;
    bool isPublic;
    VariableKind kind;
    std::string identifier;
    std::string moduleName;
    bool isRoot;
    std::shared_ptr<ValueType> valueType;
};

#endif