#include "ModulesStore.h"

#include "Module.h"

#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementVariableDeclaration.h"
#include "Parser/ValueType.h"

ModulesStore::ModulesStore(string defaultModuleName) :
defaultModuleName(defaultModuleName) { }

//
// Private
//

shared_ptr<ValueType> ModulesStore::typeForExportedStatementFromType(shared_ptr<ValueType> valueType, string moduleName) {
    switch (valueType->getKind()) {
        case ValueTypeKind::BLOB: {
            string name = *(valueType->getBlobName());
            if (name.find('.', 0) == string::npos && defaultModuleName.compare(moduleName) != 0) {
                name = moduleName + "." + name;
            }
            return ValueType::blob(name);
        }
        case ValueTypeKind::DATA:
            return ValueType::data(typeForExportedStatementFromType(valueType->getSubType(), moduleName), valueType->getCountExpression());
        case ValueTypeKind::PTR:
            return ValueType::ptr(typeForExportedStatementFromType(valueType->getSubType(), moduleName));
        case ValueTypeKind::FUN: {
            // first convert each of the argument types
            vector<shared_ptr<ValueType>> argumentTypes = *(valueType->getArgumentTypes());
            vector<shared_ptr<ValueType>> exportedArgumentTypes;
            for (shared_ptr<ValueType> argumentType : argumentTypes)
                exportedArgumentTypes.push_back(typeForExportedStatementFromType(argumentType, moduleName));
            // then the return type
            shared_ptr<ValueType> exportedReturnType = typeForExportedStatementFromType(valueType->getReturnType(), moduleName);
            // and finally return a new function type
            return ValueType::fun(exportedArgumentTypes, exportedReturnType);
        }
        default:
            return valueType;
    }
}

//
// Public
//

void ModulesStore::appendStatements(vector<shared_ptr<Statement>> statements) {
    string moduleName = defaultModuleName;

    vector<shared_ptr<Statement>> moduleImportStatements;
    vector<shared_ptr<Statement>> moduleBlobStatements;
    vector<shared_ptr<Statement>> moduleBlobDeclarationStatements;
    vector<shared_ptr<Statement>> moduleFunctionDeclarationStatements;
    vector<shared_ptr<Statement>> moduleVariableDeclarationStatements;
    vector<shared_ptr<Statement>> moduleBodyStatements;

    vector<shared_ptr<Statement>> moduleExportedBlobStatements;
    vector<shared_ptr<Statement>> moduleExportedBlobDeclarationStatements;
    vector<shared_ptr<Statement>> moduleExportedVariableDeclarationStatements;
    vector<shared_ptr<Statement>> moduleExportedFunctionDeclarationStatements;

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
                // exported header
                if (statementBlob->getShouldExport()) {
                    // update member variable statements for exported statement
                    vector<shared_ptr<StatementVariable>> exportedVariableStatements;
                    for (shared_ptr<StatementVariable> statementVariable : statementBlob->getVariableStatements()) {
                        shared_ptr<StatementVariable> exportedVariableStatement = make_shared<StatementVariable>(
                            statementVariable->getShouldExport(),
                            statementVariable->getIdentifier(),
                            typeForExportedStatementFromType(statementVariable->getValueType(), moduleName),
                            statementVariable->getExpression(),
                            statementVariable->getLocation()
                        );
                        exportedVariableStatements.push_back(exportedVariableStatement);
                    }

                    shared_ptr<StatementBlob> exportedStatementBlob = make_shared<StatementBlob>(
                        statementBlob->getShouldExport(),
                        statementBlob->getName(),
                        exportedVariableStatements,
                        vector<shared_ptr<StatementFunction>>(), // don't include function definitions
                        statementBlob->getLocation()
                    );

                    // append updated statement
                    moduleExportedBlobStatements.push_back(exportedStatementBlob);
                    // declaration doesn't contain any types, so it's fine like this
                    moduleExportedBlobDeclarationStatements.push_back(statementBlobDeclaration);
                }

                // create delclarations for blob functions
                for (shared_ptr<StatementFunction> statementBlobFunction : statementBlob->getFunctionStatements()) {
                    shared_ptr<StatementFunctionDeclaration> statementBlobFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                        false,
                        statementBlobFunction->getName(),
                        statementBlobFunction->getArguments(),
                        statementBlobFunction->getReturnValueType(),
                        statementBlobFunction->getLocation()
                    );
                    moduleFunctionDeclarationStatements.push_back(statementBlobFunctionDeclaration);

                    // handle exported & public functions
                    if (statementBlob->getShouldExport())
                        moduleExportedFunctionDeclarationStatements.push_back(statementBlobFunctionDeclaration);
                }
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
                    moduleExportedFunctionDeclarationStatements.push_back(exportedStatementFunctionDeclaration);
                }
                break;
            }
            case StatementKind::META_IMPORT: {
                moduleImportStatements.push_back(statement);
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
                moduleBodyStatements.push_back(statementVariable);
                // local header
                moduleVariableDeclarationStatements.push_back(statementVariableDeclaration);
                // exported header
                if (statementVariable->getShouldExport()) {
                    // updated variable type for exported statement
                    shared_ptr<ValueType> valueType = typeForExportedStatementFromType(statementVariableDeclaration->getValueType(), moduleName);

                    // new declaration with updated type
                    shared_ptr<StatementVariableDeclaration> exportedStatementVariableDeclaration = make_shared<StatementVariableDeclaration>(
                        statementVariableDeclaration->getShouldExport(),
                        statementVariableDeclaration->getIdentifier(),
                        valueType,
                        statementVariableDeclaration->getLocation()
                    );
                    moduleExportedVariableDeclarationStatements.push_back(statementVariableDeclaration);
                }
                break;
            }
            default: {
                moduleBodyStatements.push_back(statement);
                break;
            }
        }
    }

    // Merge with existing data
    // create new entries
    if (find(moduleNames.begin(), moduleNames.end(), moduleName) == moduleNames.end()) {
        // name
        moduleNames.push_back(moduleName);

        // imports
        importStatementsMap[moduleName] = moduleImportStatements;
        // blob declarations
        blobDeclarationStatementsMap[moduleName] = moduleBlobDeclarationStatements;
        // blob defintions
        blobStatementsMap[moduleName] = moduleBlobStatements;
        // variable declarations
        variableDeclarationStatementsMap[moduleName] = moduleVariableDeclarationStatements;
        // function declarations
        functionDeclarationStatementsMap[moduleName] = moduleFunctionDeclarationStatements;
    
        // body statements
        bodyStatementsMap[moduleName] = moduleBodyStatements;

        // exported blob declarations
        exportedBlobDeclarationStatementsMap[moduleName] = moduleExportedBlobDeclarationStatements;
        // exported blob definitions
        exportedBlobStatementsMap[moduleName] = moduleExportedBlobStatements;
        // exported variable declarations
        exportedVariableDeclarationStatementsMap[moduleName] = moduleExportedVariableDeclarationStatements;
        // exported function declarations
        exportedFunctionDeclarationStatementsMap[moduleName] = moduleExportedFunctionDeclarationStatements;
    // or merge with existing ones
    } else {
        // imports
        for (shared_ptr<Statement> statement : moduleImportStatements)
            importStatementsMap[moduleName].push_back(statement);
        // blob declarations
        for (shared_ptr<Statement> statement : moduleBlobDeclarationStatements)
            blobDeclarationStatementsMap[moduleName].push_back(statement);
        // blob defintions
        for (shared_ptr<Statement> statement : moduleBlobStatements)
            blobStatementsMap[moduleName].push_back(statement);
        // variable declarations
        for (shared_ptr<Statement> statement : moduleVariableDeclarationStatements)
            variableDeclarationStatementsMap[moduleName].push_back(statement);
        // function declarations
        for (shared_ptr<Statement> statement : moduleFunctionDeclarationStatements)
            functionDeclarationStatementsMap[moduleName].push_back(statement);

        // body statements
        for (shared_ptr<Statement> statement : moduleBodyStatements)
            bodyStatementsMap[moduleName].push_back(statement);

        // exported blob declarations
        for (shared_ptr<Statement> statement : moduleExportedBlobDeclarationStatements)
            exportedBlobDeclarationStatementsMap[moduleName].push_back(statement);
        // exported blob defintions
        for (shared_ptr<Statement> statement : moduleExportedBlobStatements)
            exportedBlobStatementsMap[moduleName].push_back(statement);
        // exported variable declarations
        for (shared_ptr<Statement> statement : moduleExportedVariableDeclarationStatements)
            exportedVariableDeclarationStatementsMap[moduleName].push_back(statement);
        // exported function declarations
        for (shared_ptr<Statement> statement : moduleExportedFunctionDeclarationStatements)
            exportedFunctionDeclarationStatementsMap[moduleName].push_back(statement);
    }
}

vector<shared_ptr<Module>> ModulesStore::getModules() {
    vector<shared_ptr<Module>> modules;

    for (string &moduleName : moduleNames) {
        // construct the local header
        // order for local header statements is:
        // - import statements
        // - blob declarations
        // - blob definitions
        // - variable declarations
        // - function declarations

        vector<shared_ptr<Statement>> headerStatements;
        // imports
        for (shared_ptr<Statement> Statement : importStatementsMap[moduleName])
            headerStatements.push_back(Statement);
        // blob declarations
        for (shared_ptr<Statement> statement : blobDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // blob definitions
        for (shared_ptr<Statement> statement : blobStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // variable declarations
        for (shared_ptr<Statement> statement : variableDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // function declarations
        for (shared_ptr<Statement> statement : functionDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);

        // finally construct the module
        shared_ptr<Module> module = make_shared<Module>(
            moduleName,
            headerStatements,
            bodyStatementsMap[moduleName]
        );
        modules.push_back(module);
    }

    return modules;
}

map<string, vector<shared_ptr<Statement>>> ModulesStore::getExportedHeaderStatementsMap() {
    // construct the exported headers map
    // it is shared by all the modules

    // order for exported header statements is:
    // - blob declarations
    // - blob definitions
    // - variable declarations
    // - function declarations
    map<string, vector<shared_ptr<Statement>>> statementsMap;
    for (string &moduleName : moduleNames) {
        // exported blob declarations
        for (shared_ptr<Statement> statement : exportedBlobDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported blob definitions
        for (shared_ptr<Statement> statement : exportedBlobStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported variable declarations
        for (shared_ptr<Statement> statement : exportedVariableDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported function declarations
        for (shared_ptr<Statement> statement : exportedFunctionDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
    }

    return statementsMap;
}
