#ifndef STATEMENT_PROTO_H
#define STATEMENT_PROTO_H

#include "Statement.h"

class StatementProtoDeclaration;
class StatementVariableDeclaration;
class StatementFunctionDeclaration;
class SymbolName;

class StatementProto: public Statement {
public:
    StatementProto(
        bool shouldExport,
        const std::string &name,
        const std::vector<std::shared_ptr<StatementVariableDeclaration>> &statementVariableDeclarations,
        const std::vector<std::shared_ptr<StatementFunctionDeclaration>> &statementFunctionDeclarations,
        std::shared_ptr<Location> location
    );

    bool getShouldExport() const;
    std::shared_ptr<SymbolName> getSymbolName() const;
    void setModuleName(const std::string &moduleName);

    std::vector<std::shared_ptr<StatementVariableDeclaration>> getStatementVariableDeclarations() const;
    std::vector<std::shared_ptr<StatementFunctionDeclaration>> getStatementFunctionDeclarations() const;

    std::shared_ptr<StatementProtoDeclaration> getDeclaration() const;

private:
    bool shouldExport;
    std::shared_ptr<SymbolName> symbolName;
    std::vector<std::shared_ptr<StatementVariableDeclaration>> statementVariableDeclarations;
    std::vector<std::shared_ptr<StatementFunctionDeclaration>> statementFunctionDeclarations;
};

#endif