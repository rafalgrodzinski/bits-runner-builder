#ifndef MODULES_STORE_H
#define MODULES_STORE_H

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

class DependencyNode;
class Error;
class Location;
class Module;
class Expression;
class Statement;
class StatementBlob;
class StatementMetaImport;
class StatementProto;
class StatementVariableDeclaration;

class ModulesStore {
public:
    ModulesStore(const std::string &defaultModuleName);

    void appendStatements(std::vector<std::shared_ptr<Statement>> statements);
    std::vector<std::shared_ptr<Module>> getModules();

private:
    std::vector<std::shared_ptr<Error>> errors;

    std::string defaultModuleName;
    std::vector<std::string> moduleNames;

    std::map<std::string, std::vector<std::shared_ptr<Statement>>> localStatementsMap;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> exportedStatementsMap;

    void setModuleName(std::shared_ptr<Statement> statement, const std::string &moduleName, bool isRoot = false);
    void setModuleName(std::shared_ptr<Expression> expression, const std::string &moduleName);

    std::vector<std::string> visitedModules;

    std::vector<std::shared_ptr<Statement>> sortedStatements(const std::string &moduleName);
    std::shared_ptr<DependencyNode> dependencyNode(std::shared_ptr<Statement> statement);
    std::shared_ptr<DependencyNode> dependencyNode(std::shared_ptr<StatementBlob> statementBlob);
    std::shared_ptr<DependencyNode> dependencyNode(std::shared_ptr<StatementMetaImport> statementMetaImport);
    std::shared_ptr<DependencyNode> dependencyNode(std::shared_ptr<StatementProto> statementProto);
    std::shared_ptr<DependencyNode> dependencyNode(std::shared_ptr<StatementVariableDeclaration> statementVariableDeclaration);

    std::vector<std::shared_ptr<Statement>> flattenedStatements(std::vector<std::shared_ptr<Statement>> statements, const std::string &moduleName);
    std::vector<std::shared_ptr<Statement>> sortedBlobProtoStatements(
        std::vector<std::shared_ptr<DependencyNode>> dependencyNodes,
        std::vector<std::shared_ptr<Statement>> flattenedModuleStatements
    );

    void markErrorDependencyCycle(std::shared_ptr<Location> location);
    void markErrorInvalidImport(std::shared_ptr<Location> location, const std::string &moduleName);
};

#endif