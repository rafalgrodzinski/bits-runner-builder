#include "ModulesStore.h"

#include "Module.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementMetaExternVariable.h"
#include "Parser/Statement/StatementMetaImport.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementProto.h"
#include "Parser/Statement/StatementProtoDeclaration.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementVariableDeclaration.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionCast.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionValue.h"

#include "Parser/ValueType/ValueType.h"

ModulesStore::ModulesStore(const string &defaultModuleName):
defaultModuleName(defaultModuleName) { }

/// Private ///

void ModulesStore::setModuleName(shared_ptr<Statement> statement, const string &moduleName, bool isRoot) {
    if (statement == nullptr)
        return;

    switch (statement->getKind()) {
        case StatementKind::ASSIGNMENT: {
            shared_ptr<StatementAssignment> statementAssignment = dynamic_pointer_cast<StatementAssignment>(statement);
            setModuleName(statementAssignment->getExpressionChained(), moduleName);
            setModuleName(statementAssignment->getValueExpression(), moduleName);
            break;
        }
        case StatementKind::BLOB: {
            shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
            statementBlob->setModuleName(moduleName);
            // variable statements
            for (shared_ptr<Statement> variableStatement : statementBlob->getVariableStatements()) {
                setModuleName(variableStatement, moduleName);
            }
            // function statements
            for (shared_ptr<Statement> functionStatement : statementBlob->getFunctionStatements()) {
                setModuleName(functionStatement, moduleName);
            }
            break;
        }
        case StatementKind::BLOCK: {
            shared_ptr<StatementBlock> statementBlock = dynamic_pointer_cast<StatementBlock>(statement);
            for (shared_ptr<Statement> blockStatement : statementBlock->getStatements()) {
                setModuleName(blockStatement, moduleName);
            }
            break;
        }
        case StatementKind::EXPRESSION: {
            shared_ptr<StatementExpression> statementExpression = dynamic_pointer_cast<StatementExpression>(statement);
            setModuleName(statementExpression->getExpression(), moduleName);
            break;
        }
        case StatementKind::FUNCTION: {
            shared_ptr<StatementFunction> statementFunction = dynamic_pointer_cast<StatementFunction>(statement);
            statementFunction->setModuleName(moduleName);
            // arguments
            for (const pair<string, shared_ptr<ValueType>> &argumentPair : statementFunction->getArguments()) {
                argumentPair.second->setModuleName(moduleName);
            }
            // return
            statementFunction->getReturnValueType()->setModuleName(moduleName);
            // body
            setModuleName(statementFunction->getStatementBlock(), moduleName);
            break;
        }
        case StatementKind::META_EXTERN_FUNCTION: {
            shared_ptr<StatementMetaExternFunction> statementMetaExternFunction = dynamic_pointer_cast<StatementMetaExternFunction>(statement);
            statementMetaExternFunction->setModuleName(moduleName);
            // arguments
            for (const pair<string, shared_ptr<ValueType>> &argumentPair : statementMetaExternFunction->getArguments()) {
                argumentPair.second->setModuleName(moduleName);
            }
            // return
            statementMetaExternFunction->getReturnValueType()->setModuleName(moduleName);
            break;
        }
        case StatementKind::META_EXTERN_VARIABLE: {
            shared_ptr<StatementMetaExternVariable> statementMetaExternVariable = dynamic_pointer_cast<StatementMetaExternVariable>(statement);
            statementMetaExternVariable->setModuleName(moduleName);
            break;
        }
        case StatementKind::PROTO: {
            shared_ptr<StatementProto> statementProto = dynamic_pointer_cast<StatementProto>(statement);
            statementProto->setModuleName(moduleName);
            // variable statements
            for (shared_ptr<Statement> variableStatement : statementProto->getVariableStatements())
                setModuleName(variableStatement, moduleName);
            // function statements
            for (shared_ptr<Statement> functionStatement : statementProto->getFunctionDeclarationStatements())
                setModuleName(functionStatement, moduleName);
            break;
        }
        case StatementKind::RAW_FUNCTION: {
            shared_ptr<StatementRawFunction> statementRawFunction = dynamic_pointer_cast<StatementRawFunction>(statement);
            statementRawFunction->setModuleName(moduleName);
            break;
        }
        case StatementKind::REPEAT: {
            shared_ptr<StatementRepeat> statementRepeat = dynamic_pointer_cast<StatementRepeat>(statement);
            setModuleName(statementRepeat->getInitStatement(), moduleName);
            setModuleName(statementRepeat->getBodyBlockStatement(), moduleName);
            setModuleName(statementRepeat->getPostStatement(), moduleName);
            setModuleName(statementRepeat->getPreConditionExpression(), moduleName);
            setModuleName(statementRepeat->getPostConditionExpression(), moduleName);
            break;
        }
        case StatementKind::RETURN: {
            shared_ptr<StatementReturn> statementReturn = dynamic_pointer_cast<StatementReturn>(statement);
            setModuleName(statementReturn->getExpression(), moduleName);
            break;
        }
        case StatementKind::VARIABLE: {
            shared_ptr<StatementVariable> statementVariable = dynamic_pointer_cast<StatementVariable>(statement);
            statementVariable->setIsRoot(isRoot);
            statementVariable->getValueType()->setModuleName(moduleName);
            statementVariable->setModuleName(moduleName);
            statementVariable->getValueType()->setModuleName(moduleName);
            setModuleName(statementVariable->getExpression(), moduleName);
            break;
        }
        default:
            break;
    }
}

void ModulesStore::setModuleName(shared_ptr<Expression> expression, const string &moduleName) {
    if (expression == nullptr)
        return;

    switch (expression->getKind()) {
        case ExpressionKind::BINARY: {
            shared_ptr<ExpressionBinary> expressionBinary = dynamic_pointer_cast<ExpressionBinary>(expression);
            setModuleName(expressionBinary->getLeft(), moduleName);
            setModuleName(expressionBinary->getRight(), moduleName);
            break;
        }
        case ExpressionKind::BLOCK: {
            shared_ptr<ExpressionBlock> expressionBlock = dynamic_pointer_cast<ExpressionBlock>(expression);
            setModuleName(expressionBlock->getStatementBlock(), moduleName);
            setModuleName(expressionBlock->getResultStatementExpression(), moduleName);
            break;
        }
        case ExpressionKind::CALL: {
            shared_ptr<ExpressionCall> expressionCall = dynamic_pointer_cast<ExpressionCall>(expression);
            for (shared_ptr<Expression> argumentExpression : expressionCall->getArgumentExpressions()) {
                setModuleName(argumentExpression, moduleName);
            }
            break;
        }
        case ExpressionKind::CAST: {
            shared_ptr<ExpressionCast> expressionCast = dynamic_pointer_cast<ExpressionCast>(expression);
            expressionCast->getValueType()->setModuleName(moduleName);
            break;
        }
        case ExpressionKind::CHAINED: {
            shared_ptr<ExpressionChained> expressionChained = dynamic_pointer_cast<ExpressionChained>(expression);
            for (shared_ptr<Expression> expression : expressionChained->getChainExpressions()) {
                setModuleName(expression, moduleName);
            }
            break;
        }
        case ExpressionKind::COMPOSITE_LITERAL: {
            shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(expression);
            for (shared_ptr<Expression> expression : expressionCompositeLiteral->getExpressions()) {
                setModuleName(expression, moduleName);
            }
            break;
        }
        case ExpressionKind::GROUPING: {
            shared_ptr<ExpressionGrouping> expressionGrouping = dynamic_pointer_cast<ExpressionGrouping>(expression);
            setModuleName(expressionGrouping->getSubExpression(), moduleName);
            break;
        }
        case ExpressionKind::IF_ELSE: {
            shared_ptr<ExpressionIfElse> expressionIfElse = dynamic_pointer_cast<ExpressionIfElse>(expression);
            setModuleName(expressionIfElse->getConditionExpression(), moduleName);
            setModuleName(expressionIfElse->getThenExpression(), moduleName);
            setModuleName(expressionIfElse->getElseExpression(), moduleName);
            break;
        }
        case ExpressionKind::UNARY: {
            shared_ptr<ExpressionUnary> expressionUnary = dynamic_pointer_cast<ExpressionUnary>(expression);
            setModuleName(expressionUnary->getSubExpression(), moduleName);
            break;
        }
        case ExpressionKind::VALUE: {
            shared_ptr<ExpressionValue> expressionValue = dynamic_pointer_cast<ExpressionValue>(expression);
            if (expressionValue->getIndexExpression() != nullptr)
                setModuleName(expressionValue->getIndexExpression(), moduleName);
            break;
        }
        default:
            break;
    }
}

/// Public ///

void ModulesStore::appendStatements(vector<shared_ptr<Statement>> statements) {
    string moduleName = defaultModuleName;

    vector<shared_ptr<Statement>> moduleImportStatements;
    vector<shared_ptr<Statement>> moduleExternStatements;
    vector<shared_ptr<Statement>> moduleProtoDeclarationStatements;
    vector<shared_ptr<Statement>> moduleProtoStatements;
    vector<shared_ptr<Statement>> moduleBlobDeclarationStatements;
    vector<shared_ptr<Statement>> moduleBlobStatements;
    vector<shared_ptr<Statement>> moduleVariableDeclarationStatements;
    vector<shared_ptr<Statement>> moduleVariableStatements;
    vector<shared_ptr<Statement>> moduleFunctionDeclarationStatements;
    vector<shared_ptr<Statement>> moduleRawFunctionStatements;
    vector<shared_ptr<Statement>> moduleBodyStatements;

    vector<shared_ptr<Statement>> moduleExportedProtoDeclarationStatements;
    vector<shared_ptr<Statement>> moduleExportedProtoStatements;
    vector<shared_ptr<Statement>> moduleExportedBlobDeclarationStatements;
    vector<shared_ptr<Statement>> moduleExportedBlobStatements;
    vector<shared_ptr<Statement>> moduleExportedFunctionDeclarationStatements;
    vector<shared_ptr<Statement>> moduleExportedVariableDeclarationStatements;
    vector<shared_ptr<Statement>> moduleExportedRawFunctionStatements;

    for (shared_ptr<Statement> statement : statements) {
        setModuleName(statement, moduleName, true);

        switch (statement->getKind()) {
            case StatementKind::BLOB: {
                shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
                shared_ptr<StatementBlobDeclaration> statementBlobDeclaration = statementBlob->getDeclaration();

                // local header
                moduleBlobDeclarationStatements.push_back(statementBlobDeclaration);
                moduleBlobStatements.push_back(statementBlob);

                // exported header
                if (statementBlob->getShouldExport()) {
                    // update proto conformation for exported statement
                    vector<string> exportedProtoNames;
                    for (string &protoName : statementBlob->getProtoNames()) {
                        string name;
                        if (protoName.find('.', 0) == string::npos && defaultModuleName.compare(moduleName) != 0) {
                            name = moduleName + "." + protoName;
                        } else {
                            name = protoName;
                        }
                        exportedProtoNames.push_back(name);
                    }

                    shared_ptr<StatementBlob> exportedStatementBlob = make_shared<StatementBlob>(
                        statementBlob->getShouldExport(),
                        statementBlob->getName(),
                        statementBlob->getNamedTypeKeys(),
                        exportedProtoNames,
                        statementBlob->getVariableStatements(),
                        vector<shared_ptr<StatementFunction>>(), // don't include function definitions
                        statementBlob->getLocation()
                    );
                    exportedStatementBlob->setModuleName(statementBlob->getModuleName());

                    // append updated statement
                    moduleExportedBlobStatements.push_back(exportedStatementBlob);
                    // declaration doesn't contain any types, so it's fine like this
                    moduleExportedBlobDeclarationStatements.push_back(statementBlobDeclaration);
                }

                // create delclarations for blob functions
                for (shared_ptr<StatementFunction> statementBlobFunction : statementBlob->getFunctionStatements()) {
                    shared_ptr<StatementFunctionDeclaration> statementBlobFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                        statementBlob->getShouldExport(),
                        statementBlobFunction->getName(),
                        statementBlobFunction->getModuleName(),
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
                shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = statementFunction->getDeclaration();

                // local header
                moduleFunctionDeclarationStatements.push_back(statementFunctionDeclaration);
                // body
                moduleBodyStatements.push_back(statementFunction);

                // exported header
                if (statementFunction->getShouldExport())
                   moduleExportedFunctionDeclarationStatements.push_back(statementFunctionDeclaration);

                break;
            }
            case StatementKind::META_EXTERN_FUNCTION:
            case StatementKind::META_EXTERN_VARIABLE: {
                moduleExternStatements.push_back(statement);
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
            case StatementKind::PROTO: {
                shared_ptr<StatementProto> statementProto = dynamic_pointer_cast<StatementProto>(statement);
                shared_ptr<StatementProtoDeclaration> statementProtoDeclaration = statementProto->getDeclaration();

                // local header
                moduleProtoDeclarationStatements.push_back(statementProtoDeclaration);
                moduleProtoStatements.push_back(statementProto);

                // exported header
                if (statementProto->getShouldExport()) {
                    // append proto statement
                    moduleExportedProtoStatements.push_back(statementProto);
                    // declaration doesn't contain any types, so it's fine like this
                    moduleExportedProtoDeclarationStatements.push_back(statementProtoDeclaration);
                }

                break;
            }
            case StatementKind::RAW_FUNCTION: {
                shared_ptr<StatementRawFunction> statementRawFunction = dynamic_pointer_cast<StatementRawFunction>(statement);
                moduleRawFunctionStatements.push_back(statementRawFunction);
                if (statementRawFunction->getShouldExport())
                    moduleExportedRawFunctionStatements.push_back(statementRawFunction);

                break;
            }
            case StatementKind::VARIABLE: {
                shared_ptr<StatementVariable> statementVariable = dynamic_pointer_cast<StatementVariable>(statement);
                shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = statementVariable->getDeclaration();

                // local header
                moduleVariableStatements.push_back(statementVariable);

                // exported header
                if (statementVariable->getShouldExport())
                   moduleExportedVariableDeclarationStatements.push_back(statementVariableDeclaration);

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
        // externs
        externStatementsMap[moduleName] = moduleExternStatements;
        // proto declarations
        protoDeclarationStatementsMap[moduleName] = moduleProtoDeclarationStatements;
        // proto definitions
        protoStatementsMap[moduleName] = moduleProtoStatements;
        // blob declarations
        blobDeclarationStatementsMap[moduleName] = moduleBlobDeclarationStatements;
        // blob definitons
        blobStatementsMap[moduleName] = moduleBlobStatements;
        // function declarations
        functionDeclarationStatementsMap[moduleName] = moduleFunctionDeclarationStatements;
        // variable definitions
        variableStatementsMap[moduleName] = moduleVariableStatements;
        // raw functions
        rawFunctionStatementsMap[moduleName] = moduleRawFunctionStatements;
    
        // body statements
        bodyStatementsMap[moduleName] = moduleBodyStatements;

        // exported proto declarations
        exportedProtoDeclarationStatementsMap[moduleName] = moduleExportedProtoDeclarationStatements;
        // exported proto definitions
        exportedProtoStatementsMap[moduleName] = moduleExportedProtoStatements;
        // exported blob declarations
        exportedBlobDeclarationStatementsMap[moduleName] = moduleExportedBlobDeclarationStatements;
        // exported blob definitions
        exportedBlobStatementsMap[moduleName] = moduleExportedBlobStatements;
        // exported function declarations
        exportedFunctionDeclarationStatementsMap[moduleName] = moduleExportedFunctionDeclarationStatements;
        // exported variable declarations
        exportedVariableDeclarationStatementsMap[moduleName] = moduleExportedVariableDeclarationStatements;
        // exported raw functions
        exportedRawFunctionStatementsMap[moduleName] = moduleExportedRawFunctionStatements;
    // or merge with existing ones
    } else {
        // imports
        for (shared_ptr<Statement> statement : moduleImportStatements) {
            // Filter out dumplicated import statements
            bool isAlreadyImported = false;
            string newImportName = dynamic_pointer_cast<StatementMetaImport>(statement)->getName();
            for (shared_ptr<Statement> importStatement : importStatementsMap[moduleName]) {
                string importName = dynamic_pointer_cast<StatementMetaImport>(importStatement)->getName();
                if (newImportName.compare(importName) == 0) {
                    isAlreadyImported = true;
                    break;
                }
            }
            if (!isAlreadyImported)
                importStatementsMap[moduleName].push_back(statement);
        }
        // externs
        for (shared_ptr<Statement> statement : moduleExternStatements)
            externStatementsMap[moduleName].push_back(statement);
        // proto declarations
        for (shared_ptr<Statement> statement : moduleProtoDeclarationStatements)
            protoDeclarationStatementsMap[moduleName].push_back(statement);
        // proto defintions
        for (shared_ptr<Statement> statement : moduleProtoStatements)
            protoStatementsMap[moduleName].push_back(statement);
        // blob declarations
        for (shared_ptr<Statement> statement : moduleBlobDeclarationStatements)
            blobDeclarationStatementsMap[moduleName].push_back(statement);
        // blob defintions
        for (shared_ptr<Statement> statement : moduleBlobStatements)
            blobStatementsMap[moduleName].push_back(statement);
        // function declarations
        for (shared_ptr<Statement> statement : moduleFunctionDeclarationStatements)
            functionDeclarationStatementsMap[moduleName].push_back(statement);
        // variable definitions
        for (shared_ptr<Statement> statement : moduleVariableStatements)
            variableStatementsMap[moduleName].push_back(statement);
        // raw functions
        for (shared_ptr<Statement> statement : moduleRawFunctionStatements)
            rawFunctionStatementsMap[moduleName].push_back(statement);

        // body statements
        for (shared_ptr<Statement> statement : moduleBodyStatements)
            bodyStatementsMap[moduleName].push_back(statement);

        // exported proto declarations
        for (shared_ptr<Statement> statement : moduleExportedProtoDeclarationStatements)
            exportedBlobDeclarationStatementsMap[moduleName].push_back(statement);
        // exported proto definitions
        for (shared_ptr<Statement> statement : moduleExportedProtoStatements)
            exportedBlobStatementsMap[moduleName].push_back(statement);
        // exported blob declarations
        for (shared_ptr<Statement> statement : moduleExportedBlobDeclarationStatements)
            exportedBlobDeclarationStatementsMap[moduleName].push_back(statement);
        // exported blob defintions
        for (shared_ptr<Statement> statement : moduleExportedBlobStatements)
            exportedBlobStatementsMap[moduleName].push_back(statement);
        // exported function declarations
        for (shared_ptr<Statement> statement : moduleExportedFunctionDeclarationStatements)
            exportedFunctionDeclarationStatementsMap[moduleName].push_back(statement);
        // exported variable declarations
        for (shared_ptr<Statement> statement : moduleExportedVariableDeclarationStatements)
            exportedVariableDeclarationStatementsMap[moduleName].push_back(statement);
        // exported raw functions
        for (shared_ptr<Statement> statement : moduleExportedRawFunctionStatements)
            exportedRawFunctionStatementsMap[moduleName].push_back(statement);
    }
}

vector<shared_ptr<Module>> ModulesStore::getModules() {
    vector<shared_ptr<Module>> modules;

    for (const string &moduleName : moduleNames) {
        // construct the local header
        // order for local header statements is:
        // - externs
        // - proto declaration
        // - blob declarations
        // - import statements (imported statements may use blobs & protos)
        // - proto definition
        // - blob definitions
        // - function declarations
        // - variable definitions
        // - raw function definitions

        vector<shared_ptr<Statement>> headerStatements;
        // externs
        for (shared_ptr<Statement> statement : externStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // proto declarations
        for (shared_ptr<Statement> statement : protoDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // blob declarations
        for (shared_ptr<Statement> statement : blobDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // imports
        for (shared_ptr<Statement> statement : importStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // proto definitions
        for (shared_ptr<Statement> statement : protoStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // blob definitions
        for (shared_ptr<Statement> statement : blobStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // function declarations
        for (shared_ptr<Statement> statement : functionDeclarationStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // variable definitions
        for (shared_ptr<Statement> statement : variableStatementsMap[moduleName])
            headerStatements.push_back(statement);
        // raw functions definitions
        for (shared_ptr<Statement> statement : rawFunctionStatementsMap[moduleName])
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
    // - import statements
    // - proto declarations
    // - proto definitions
    // - blob declarations
    // - blob definitions
    // - function declarations
    // - variable declarations
    // - raw function definitions
    map<string, vector<shared_ptr<Statement>>> statementsMap;
    for (const string &moduleName : moduleNames) {
        // first initialize it with an empty array (in case there are no exported statements)
        statementsMap[moduleName] = {};

        // imports
        for (shared_ptr<Statement> statement : importStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported proto declarations
        for (shared_ptr<Statement> statement : exportedProtoDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported proto definitions
        for (shared_ptr<Statement> statement : exportedProtoStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported blob declarations
        for (shared_ptr<Statement> statement : exportedBlobDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported blob definitions
        for (shared_ptr<Statement> statement : exportedBlobStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported function declarations
        for (shared_ptr<Statement> statement : exportedFunctionDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported variable declarations
        for (shared_ptr<Statement> statement : exportedVariableDeclarationStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
        // exported raw function definitions
        for (shared_ptr<Statement> statement : exportedRawFunctionStatementsMap[moduleName])
            statementsMap[moduleName].push_back(statement);
    }

    return statementsMap;
}
