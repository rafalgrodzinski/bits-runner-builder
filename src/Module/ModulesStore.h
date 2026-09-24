#ifndef MODULES_STORE_H
#define MODULES_STORE_H

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Module;
class Expression;
class Statement;
class ValueType;

class ModulesStore {
public:
    ModulesStore(const std::string &defaultModuleName);

    void appendStatements(std::vector<std::shared_ptr<Statement>> statements);
    std::vector<std::shared_ptr<Module>> getModules();
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> getExportedHeaderStatementsMap();

private:
    std::string defaultModuleName;
    std::vector<std::string> moduleNames;

    // header
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> importStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> externStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> enumStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> protoDeclarationStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> protoStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> blobDeclarationStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> blobStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> statementVariablesMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> statementFunctionDeclarationsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> rawFunctionStatementsMap;
    // body
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> bodyStatementsMap;
    // exported
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedEnumStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedProtoDeclarationStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedProtoStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedBlobDeclarationStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedBlobStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedVariableDeclarationStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedStatementFunctionDeclarationsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedRawFunctionStatementsMap;

    void setModuleName(std::shared_ptr<Statement> statement, const std::string &moduleName, bool isRoot = false);
    void setModuleName(std::shared_ptr<Expression> expression, const std::string &moduleName);
};

#endif