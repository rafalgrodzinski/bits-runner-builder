#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include <format>
#include "Statement.h"

class StatementBlobDeclaration;
class StatementFunction;
class StatementVariableDeclaration;
class SymbolName;
class ValueType;

class StatementBlob: public Statement {
public:
    StatementBlob(
        bool shouldExport,
        const std::string &name,
        const std::vector<std::string> &namedTypeKeys,
        const std::vector<std::string> &protoNames,
        const std::vector<std::shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
        const std::vector<std::shared_ptr<StatementFunction>> &statementFunctions,
        std::shared_ptr<Location> location
    );

    StatementBlob(
        bool shouldExport,
        std::shared_ptr<SymbolName> symbolName,
        const std::vector<std::string> &namedTypeKeys,
        std::vector<std::shared_ptr<SymbolName>> conformingProtoSymbolNames,
        const std::vector<std::shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
        const std::vector<std::shared_ptr<StatementFunction>> &statementFunctions,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::shared_ptr<SymbolName> getSymbolName() const;
    std::shared_ptr<SymbolName> getPackedSymbolName() const;
    void setModuleName(const std::string &moduleName);

    std::vector<std::string> getNamedTypeKeys() const;
    std::vector<std::shared_ptr<SymbolName>> getProtoSymbolNames() const;
    std::vector<std::shared_ptr<StatementVariableDeclaration>> getStatementVariableDeclarations() const;
    std::vector<std::shared_ptr<StatementFunction>> getStatementFunctions() const;
    std::vector<std::pair<std::string, std::shared_ptr<ValueType>>> getFields() const;

    std::shared_ptr<StatementBlobDeclaration> getDeclaration() const;

private:
    bool shouldExport;
    std::shared_ptr<SymbolName> symbolName;
    std::vector<std::string> namedTypeKeys;
    std::vector<std::shared_ptr<StatementVariableDeclaration>> statementVariableDeclarations;
    std::vector<std::shared_ptr<StatementFunction>> statementFunctions;
    std::vector<std::shared_ptr<SymbolName>> protoSymbolNames;
};

#endif