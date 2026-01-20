#include "ModulesStore.h"

#include "Module.h"

#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementVariableDeclaration.h"

ModulesStore::ModulesStore(string defaultModuleName) :
defaultModuleName(defaultModuleName) { }

void ModulesStore::appendStatements(vector<shared_ptr<Statement>> statements) {
    string moduleName = defaultModuleName;
    vector<shared_ptr<Statement>> moduleBodyStatements;
    vector<shared_ptr<Statement>> moduleBlobStatements;
    vector<shared_ptr<Statement>> moduleBlobDeclarationStatements;
    vector<shared_ptr<Statement>> moduleFunctionDeclarationStatements;
    vector<shared_ptr<Statement>> moduleVariableDeclarationStatements;

    for (shared_ptr<Statement> statement : statements) {
        switch (statement->getKind()) {
            case StatementKind::BLOB: {
                shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
                shared_ptr<StatementBlobDeclaration> statementBlobDeclaration = make_shared<StatementBlobDeclaration>(
                    statementBlob->getShouldExport(),
                    statementBlob->getName(),
                    statementBlob->getLocation()
                );                        
                // local header
                moduleBlobStatements.push_back(statementBlob);
                moduleBlobDeclarationStatements.push_back(statementBlobDeclaration);
                /*
                // exported header
                if (statementBlob->getShouldExport()) {
                    // update member types for exported statement
                    vector<pair<string, shared_ptr<ValueType>>> exportedMembers;
                    for (pair<string, shared_ptr<ValueType>> member : statementBlob->getMembers())
                        exportedMembers.push_back(pair(member.first, typeForExportedStatementFromType(member.second, moduleName)));

                    shared_ptr<StatementBlob> exportedStatementBlob = make_shared<StatementBlob>(
                        statementBlob->getShouldExport(),
                        statementBlob->getName(),
                        exportedMembers,
                        statementBlob->getLocation()
                    );

                    // declaration doesn't contain any types, so it's fine like this
                    exportedHeaderStatements.push_back(statementBlobDeclaration);
                    // append updated statement
                    exportedHeaderStatements.push_back(exportedStatementBlob);
                }
                */
                break;
            }
            case StatementKind::FUNCTION: {
                shared_ptr<StatementFunction> statementFunction = dynamic_pointer_cast<StatementFunction>(statement);
                shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                    statementFunction->getShouldExport(),
                    statementFunction->getName(),
                    statementFunction->getArguments(),
                    statementFunction->getReturnValueType(),
                    statementFunction->getLocation()
                );
                // body
                moduleBodyStatements.push_back(statementFunction);
                // local header
                moduleFunctionDeclarationStatements.push_back(statementFunctionDeclaration);
                /*
                // exported header
                if (statementFunction->getShouldExport()) {
                    // update argument types for exported statement
                    vector<pair<string, shared_ptr<ValueType>>> exportedArguments;
                    for (pair<string, shared_ptr<ValueType>> argument : statementFunctionDeclaration->getArguments())
                        exportedArguments.push_back(pair(argument.first, typeForExportedStatementFromType(argument.second, moduleName)));

                    // updated return type for exported statement
                    shared_ptr<ValueType> exportedReturnValueType = typeForExportedStatementFromType(statementFunctionDeclaration->getReturnValueType(), moduleName);

                    shared_ptr<StatementFunctionDeclaration> exportedStatementFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                        statementFunctionDeclaration->getShouldExport(),
                        statementFunctionDeclaration->getName(),
                        exportedArguments,
                        exportedReturnValueType,
                        statementFunctionDeclaration->getLocation()
                    );

                    // append updated statement
                    exportedHeaderStatements.push_back(exportedStatementFunctionDeclaration);
                }
                */
                break;
            }
            case StatementKind::MODULE: {
                shared_ptr<StatementModule> statementModule = dynamic_pointer_cast<StatementModule>(statement);
                moduleName = statementModule->getName();
                break;
            }
            case StatementKind::VARIABLE: {
                shared_ptr<StatementVariable> statementVariable = dynamic_pointer_cast<StatementVariable>(statement);
                shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = make_shared<StatementVariableDeclaration>(
                    statementVariable->getShouldExport(),
                    statementVariable->getIdentifier(),
                    statementVariable->getValueType(),
                    statementVariable->getLocation()
                );
                // body
                statements.push_back(statementVariable);
                // local header
                moduleVariableDeclarationStatements.push_back(statementVariableDeclaration);
                /*
                // exported header
                if (statementVariable->getShouldExport()) {
                    // new declaration with updated type
                    shared_ptr<StatementVariableDeclaration> exportedStatementVariableDeclaration = make_shared<StatementVariableDeclaration>(
                        statementVariableDeclaration->getShouldExport(),
                        statementVariableDeclaration->getIdentifier(),
                        typeForExportedStatementFromType(statementVariableDeclaration->getValueType(), moduleName),
                        statementVariableDeclaration->getLocation()
                    );
                    exportedHeaderStatements.push_back(statementVariableDeclaration);
                }
                */
                break;
            }
            default: {
                moduleBodyStatements.push_back(statement);
                break;
            }
        }
    }

    // Merge with existing data
    if (find(moduleNames.begin(), moduleNames.end(), moduleName) == moduleNames.end()) {
        // name
        moduleNames.push_back(moduleName);
        // statements
        bodyStatementsMap[moduleName] = moduleBodyStatements;
        // blob statements
        blobStatementsMap[moduleName] = moduleBlobStatements;
        // blob declrations
        blobDeclarationStatementsMap[moduleName] = moduleBlobDeclarationStatements;
        // function declarations
        functionDeclarationStatementsMap[moduleName] = moduleFunctionDeclarationStatements;
        // variable declarations
        variableDeclarationStatementsMap[moduleName] = moduleVariableDeclarationStatements;
    } else {
        // statements
        for (shared_ptr<Statement> statement : moduleBodyStatements)
            bodyStatementsMap[moduleName].push_back(statement);
        // blob statements
        for (shared_ptr<Statement> statement : moduleBlobStatements)
            blobStatementsMap[moduleName].push_back(statement);
        // blob declarations
        for (shared_ptr<Statement> statement : moduleBlobDeclarationStatements)
            blobDeclarationStatementsMap[moduleName].push_back(statement);
        // function declarations
        for (shared_ptr<Statement> statement : moduleFunctionDeclarationStatements)
            functionDeclarationStatementsMap[moduleName].push_back(statement);
        // variable declarations
        for (shared_ptr<Statement> statement : moduleVariableDeclarationStatements)
            variableDeclarationStatementsMap[moduleName].push_back(statement);
    }
}

vector<shared_ptr<Module>> ModulesStore::getModules() {
    vector<shared_ptr<Module>> modules;

    for (string &moduleName : moduleNames) {
        // order for header statemnts is:
        // - blob declarations
        // - blob definitions
        // - function declarations
        // - variable declarations
        vector<shared_ptr<Statement>> headerStatements;
        // blob declarations
        for (shared_ptr<Statement> statement : blobDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // blob definitions
        for (shared_ptr<Statement> statement : blobStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // function declarations
        for (shared_ptr<Statement> statement : functionDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // variable declarations
        for (shared_ptr<Statement> statement : variableDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);

        // finally construct the module
        shared_ptr<Module> module = make_shared<Module>(
            moduleName,
            bodyStatementsMap[moduleName],
            headerStatements
        );
        modules.push_back(module);
    }

    return modules;
}