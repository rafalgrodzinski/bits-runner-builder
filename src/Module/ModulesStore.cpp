#include "ModulesStore.h"

#include "Module.h"
#include "DependencyNode.h"

#include "Error.h"
#include "Logger.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementAssignment.h"
#include "Parser/Statement/StatementBlob.h"
#include "Parser/Statement/StatementBlobDeclaration.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementEnum.h"
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
#include "Parser/Expression/ExpressionMatch.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionValue.h"

#include "Parser/Pattern.h"
#include "Parser/ValueType/ValueType.h"
#include "Parser/ValueType/ValueTypeBlob.h"
#include "Parser/ValueType/ValueTypeProto.h"

using namespace std;

// MARK: - Public

ModulesStore::ModulesStore(const string &defaultModuleName):
defaultModuleName(defaultModuleName) { }

void ModulesStore::appendStatements(vector<shared_ptr<Statement>> statements) {
    string moduleName = defaultModuleName;

    vector<shared_ptr<Statement>> moduleLocalStatements;
    vector<shared_ptr<Statement>> moduleExportedStatements;

    for (shared_ptr<Statement> statement : statements) {
        setModuleName(statement, moduleName, true);

        switch (statement->getKind()) {
            case StatementKind::BLOB: {
                shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
                shared_ptr<StatementBlobDeclaration> statementBlobDeclaration = statementBlob->getDeclaration();

                // local statements
                moduleLocalStatements.push_back(statementBlobDeclaration);
                moduleLocalStatements.push_back(statementBlob);

                // exported statements
                if (statementBlob->getShouldExport()) {
                    shared_ptr<StatementBlob> exportedStatementBlob = make_shared<StatementBlob>(
                        statementBlob->getShouldExport(),
                        statementBlob->getSymbolName(),
                        statementBlob->getNamedTypeKeys(),
                        statementBlob->getProtoSymbolNames(),
                        statementBlob->getStatementVariableDeclarations(),
                        vector<shared_ptr<StatementFunction>>(), // don't include function definitions
                        statementBlob->getLocation()
                    );
                    exportedStatementBlob->setModuleName(statementBlob->getSymbolName()->getModuleName());

                    // append updated statement
                    moduleExportedStatements.push_back(exportedStatementBlob);
                    // declaration doesn't contain any types, so it's fine like this
                    moduleExportedStatements.push_back(statementBlobDeclaration);
                }

                // create delclarations for blob functions
                for (shared_ptr<StatementFunction> statementBlobFunction : statementBlob->getStatementFunctions()) {
                    shared_ptr<StatementFunctionDeclaration> statementBlobFunctionDeclaration = make_shared<StatementFunctionDeclaration>(
                        statementBlob->getShouldExport(),
                        statementBlobFunction->getName(),
                        statementBlobFunction->getModuleName(),
                        statementBlobFunction->getArguments(),
                        statementBlobFunction->getReturnValueType(),
                        statementBlobFunction->getLocation()
                    );

                    // local statements
                    moduleLocalStatements.push_back(statementBlobFunctionDeclaration);

                    // handle exported & public functions
                    if (statementBlob->getShouldExport())
                       moduleExportedStatements.push_back(statementBlobFunctionDeclaration);
                }

                break;
            }
            case StatementKind::ENUM: {
                shared_ptr<StatementEnum> statementEnum = dynamic_pointer_cast<StatementEnum>(statement);

                // local statements
                moduleLocalStatements.push_back(statementEnum);

                // exported statements
                if (statementEnum->getShouldExport())
                    moduleExportedStatements.push_back(statementEnum);

                break;
            }
            case StatementKind::FUNCTION: {
                shared_ptr<StatementFunction> statementFunction = dynamic_pointer_cast<StatementFunction>(statement);
                shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = statementFunction->getDeclaration();

                // local statements
                moduleLocalStatements.push_back(statementFunctionDeclaration);
                moduleLocalStatements.push_back(statementFunction);

                // exported statements
                if (statementFunction->getShouldExport())
                   moduleExportedStatements.push_back(statementFunctionDeclaration);

                break;
            }
            case StatementKind::META_EXTERN_FUNCTION:
            case StatementKind::META_EXTERN_VARIABLE: {
                moduleLocalStatements.push_back(statement);
                break;
            }
            case StatementKind::META_IMPORT: {
                // local statements
                moduleLocalStatements.push_back(statement);

                // exported statements
                moduleExportedStatements.push_back(statement);
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

                // local statements
                moduleLocalStatements.push_back(statementProtoDeclaration);
                moduleLocalStatements.push_back(statementProto);

                // exported statements
                if (statementProto->getShouldExport()) {
                    // append proto statement
                    moduleExportedStatements.push_back(statementProto);
                    // declaration doesn't contain any types, so it's fine like this
                    moduleExportedStatements.push_back(statementProtoDeclaration);
                }

                break;
            }
            case StatementKind::RAW_FUNCTION: {
                shared_ptr<StatementRawFunction> statementRawFunction = dynamic_pointer_cast<StatementRawFunction>(statement);

                // local statements
                moduleLocalStatements.push_back(statementRawFunction);

                // exported statemetns
                if (statementRawFunction->getShouldExport())
                    moduleExportedStatements.push_back(statementRawFunction);

                break;
            }
            case StatementKind::VARIABLE: {
                shared_ptr<StatementVariable> statementVariable = dynamic_pointer_cast<StatementVariable>(statement);
                shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = statementVariable->getDeclaration();

                // local statements
                moduleLocalStatements.push_back(statementVariable);

                // exported statements
                if (statementVariable->getShouldExport())
                   moduleExportedStatements.push_back(statementVariableDeclaration);

                break;
            }
            default:
                break;
        }
    }

    // Merge with existing data
    // create new entries
    if (find(moduleNames.begin(), moduleNames.end(), moduleName) == moduleNames.end()) {
        // name
        moduleNames.push_back(moduleName);
        localStatementsMap[moduleName] = moduleLocalStatements;
        exportedStatementsMap[moduleName] = moduleExportedStatements;
    // or merge with existing ones
    } else {
        localStatementsMap[moduleName].insert(localStatementsMap[moduleName].end(), moduleLocalStatements.begin(), moduleLocalStatements.end());
        exportedStatementsMap[moduleName].insert(exportedStatementsMap[moduleName].end(), moduleExportedStatements.begin(), moduleExportedStatements.end());
    }
}

vector<shared_ptr<Module>> ModulesStore::getModules() {
    vector<shared_ptr<Module>> modules;

    for (const string &moduleName : moduleNames) {
        DependencyNode::reset();
        vector<shared_ptr<Statement>> resolvedSortedStatements = sortedStatements(moduleName);

        // finally construct the module
        shared_ptr<Module> module = make_shared<Module>(moduleName, resolvedSortedStatements);
        modules.push_back(module);
    }

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }

    return modules;
}

// MARK: - Private

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
            // statement variable declarations
            for (shared_ptr<Statement> statementVariableDeclaration : statementBlob->getStatementVariableDeclarations()) {
                setModuleName(statementVariableDeclaration, moduleName);
            }
            // statement functions
            for (shared_ptr<Statement> statementFunction : statementBlob->getStatementFunctions()) {
                setModuleName(statementFunction, moduleName);
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
        case StatementKind::ENUM: {
            shared_ptr<StatementEnum> statementEnum = dynamic_pointer_cast<StatementEnum>(statement);
            statementEnum->setModuleName(moduleName);
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
        case StatementKind::FUNCTION_DECLARATION: {
            shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration = dynamic_pointer_cast<StatementFunctionDeclaration>(statement);
            statementFunctionDeclaration->setModuleName(moduleName);
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
            // statement variable declarations
            for (shared_ptr<Statement> statementVariableDeclaration : statementProto->getStatementVariableDeclarations())
                setModuleName(statementVariableDeclaration, moduleName);
            // statement function declarations
            for (shared_ptr<Statement> statementFunctionDeclaration : statementProto->getStatementFunctionDeclarations())
                setModuleName(statementFunctionDeclaration, moduleName);
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
            statementVariable->setModuleName(moduleName);
            statementVariable->getValueType()->setModuleName(moduleName);
            setModuleName(statementVariable->getExpression(), moduleName);
            break;
        }
        case StatementKind::VARIABLE_DECLARATION: {
            shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(statement);
            statementVariableDeclaration->setModuleName(moduleName);
            statementVariableDeclaration->getValueType()->setModuleName(moduleName);
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
        case ExpressionKind::MATCH: {
            shared_ptr<ExpressionMatch> expressionMatch = dynamic_pointer_cast<ExpressionMatch>(expression);
            setModuleName(expressionMatch->getSwitchExpression(), moduleName);
            for (pair<shared_ptr<Pattern>, shared_ptr<Expression>> &casePair : expressionMatch->getCasePairs()) {
                casePair.first->getValueType()->setModuleName(moduleName);
                setModuleName(casePair.second, moduleName);
            }
            setModuleName(expressionMatch->getElseExpression(), moduleName);
            break;
        }
        case ExpressionKind::UNARY: {
            shared_ptr<ExpressionUnary> expressionUnary = dynamic_pointer_cast<ExpressionUnary>(expression);
            setModuleName(expressionUnary->getSubExpression(), moduleName);
            break;
        }
        case ExpressionKind::VALUE: {
            shared_ptr<ExpressionValue> expressionValue = dynamic_pointer_cast<ExpressionValue>(expression);
            if (expressionValue->getValueType() != nullptr)
                expressionValue->getValueType()->setModuleName(moduleName);
            if (expressionValue->getIndexExpression() != nullptr)
                setModuleName(expressionValue->getIndexExpression(), moduleName);
            break;
        }
        default:
            break;
    }
}

vector<shared_ptr<Statement>> ModulesStore::sortedStatements(const std::string &moduleName) {
    vector<shared_ptr<DependencyNode>> dependencyNodes;
    for (shared_ptr<Statement> statement : localStatementsMap[moduleName]) {
        visitedModules.clear();

        if (shared_ptr<DependencyNode> resolvedDependencyNode = dependencyNode(statement)) {
            if (resolvedDependencyNode->getValueType() != nullptr) {
                dependencyNodes.push_back(resolvedDependencyNode);
            } else {
                for (shared_ptr<DependencyNode> resolvedChildDependencyNode : resolvedDependencyNode->getChildDependencyNodes())
                    dependencyNodes.push_back(resolvedChildDependencyNode);
            }
        }
    }

    visitedModules.clear();
    vector<shared_ptr<Statement>> flattenedModuleStatements = flattenedStatements(localStatementsMap[moduleName], moduleName);
    vector<shared_ptr<Statement>> resolvedSortedBlobProtoStatements = sortedBlobProtoStatements(dependencyNodes, flattenedModuleStatements);

    // Split statements by category
    vector<shared_ptr<Statement>> statementExterns;
    vector<shared_ptr<Statement>> statementEnums;
    vector<shared_ptr<Statement>> statementProtoDeclarations;
    vector<shared_ptr<Statement>> statementBlobDeclarations;
    vector<shared_ptr<Statement>> statementVariableDeclarations;
    vector<shared_ptr<Statement>> statementVariables;
    vector<shared_ptr<Statement>> statementFunctionDeclarations;
    vector<shared_ptr<Statement>> statementRawFunctions;
    vector<shared_ptr<Statement>> statementFunctions;

    for (shared_ptr<Statement> statement : flattenedModuleStatements) {
        switch (statement->getKind()) {
            case StatementKind::META_EXTERN_VARIABLE:
            case StatementKind::META_EXTERN_FUNCTION:
                statementExterns.push_back(statement);
                break;
            case StatementKind::ENUM:
                statementEnums.push_back(statement);
                break;
            case StatementKind::PROTO_DECLARATION:
                statementProtoDeclarations.push_back(statement);
                break;
            case StatementKind::BLOB_DECLARATION:
                statementBlobDeclarations.push_back(statement);
                break;
            case StatementKind::FUNCTION_DECLARATION:
                statementFunctionDeclarations.push_back(statement);
                break;
            case StatementKind::VARIABLE_DECLARATION:
                statementVariableDeclarations.push_back(statement);
                break;
            case StatementKind::VARIABLE:
                statementVariables.push_back(statement);
                break;
            case StatementKind::RAW_FUNCTION:
                statementRawFunctions.push_back(statement);
                break;
            case StatementKind::FUNCTION:
                statementFunctions.push_back(statement);
                break;
            default:
                break; // Ignore all the other statements (imports, proto, blob)
        }
    }

    // Reconstruct the final list of statements for this module
    vector<shared_ptr<Statement>> resolvedSortedStatements;
    // order for statements is:
    // - externs
    // - enum definitions
    // - proto declaration
    // - blob declarations
    // - mix of ordered proto and blob definitions
    // - function declarations
    // - variable declarations
    // - variable definitions
    // - raw function definitions
    // - function definitions
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementExterns.begin(), statementExterns.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementEnums.begin(), statementEnums.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementProtoDeclarations.begin(), statementProtoDeclarations.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementBlobDeclarations.begin(), statementBlobDeclarations.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), resolvedSortedBlobProtoStatements.begin(), resolvedSortedBlobProtoStatements.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementFunctionDeclarations.begin(), statementFunctionDeclarations.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementVariableDeclarations.begin(), statementVariableDeclarations.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementVariables.begin(), statementVariables.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementRawFunctions.begin(), statementRawFunctions.end());
    resolvedSortedStatements.insert(resolvedSortedStatements.end(), statementFunctions.begin(), statementFunctions.end());

    return resolvedSortedStatements;
}

shared_ptr<DependencyNode> ModulesStore::dependencyNode(shared_ptr<Statement> statement) {
    switch (statement->getKind()) {
        case StatementKind::META_IMPORT:
            return dependencyNode(dynamic_pointer_cast<StatementMetaImport>(statement));
        case StatementKind::PROTO:
            return dependencyNode(dynamic_pointer_cast<StatementProto>(statement));
        case StatementKind::BLOB:
            return dependencyNode(dynamic_pointer_cast<StatementBlob>(statement));
        case StatementKind::VARIABLE_DECLARATION:
            return dependencyNode(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
        default:
            break;
    }

    return nullptr;
}

shared_ptr<DependencyNode> ModulesStore::dependencyNode(shared_ptr<StatementBlob> statementBlob) {
    vector<shared_ptr<DependencyNode>> resolvedDependencyNodes;

    // Variables
    for (shared_ptr<Statement> statement : statementBlob->getStatementVariableDeclarations()) {
        if (shared_ptr<DependencyNode> resolvedDependencyNode = dependencyNode(statement))
            resolvedDependencyNodes.push_back(resolvedDependencyNode);
    }

    // Implemented protos
    for (shared_ptr<SymbolName> protoSymbolName : statementBlob->getProtoSymbolNames()) {
        shared_ptr<ValueType> valueTypeProto = make_shared<ValueTypeProto>(protoSymbolName->getGlobalName(), statementBlob->getLocation());
        resolvedDependencyNodes.push_back(DependencyNode::dependencyNode(valueTypeProto, {}));
    }

    shared_ptr<ValueType> valueTypeBlob = make_shared<ValueTypeBlob>(
        statementBlob->getSymbolName()->getGlobalName(),
        vector<shared_ptr<ValueType>>(),
        false,
        statementBlob->getLocation()
    );
    return DependencyNode::dependencyNode(valueTypeBlob, resolvedDependencyNodes);
}

shared_ptr<DependencyNode> ModulesStore::dependencyNode(shared_ptr<StatementMetaImport> statementMetaImport) {
    // Check if module already visited
    for (const string &visitedModuleName : visitedModules) {
        if (statementMetaImport->getName() == visitedModuleName)
            return DependencyNode::dependencyNode(nullptr, {});
    }
    // Add to visited list
    visitedModules.push_back(statementMetaImport->getName());

    // check if the import is valid
    bool isValid = false;
    for (const string &moduleName : moduleNames) {
        if (moduleName == statementMetaImport->getName())
            isValid = true;
    }
    if (!isValid) {
        markErrorInvalidImport(statementMetaImport->getLocation(), statementMetaImport->getName());
        return DependencyNode::dependencyNode(nullptr, {});
    }
    
    vector<shared_ptr<DependencyNode>> resolvedDependencyNodes;

    vector<shared_ptr<Statement>> importableStatements = exportedStatementsMap[statementMetaImport->getName()];
    for (shared_ptr<Statement> statement : importableStatements) {
        if (shared_ptr<DependencyNode> resolvedDependencyNode = dependencyNode(statement))
            resolvedDependencyNodes.push_back(resolvedDependencyNode);
    }

    return DependencyNode::dependencyNode(nullptr, resolvedDependencyNodes);
}

std::shared_ptr<DependencyNode> ModulesStore::dependencyNode(std::shared_ptr<StatementProto> statementProto) {
    vector<shared_ptr<DependencyNode>> resolvedDependencyNodes;

    // Iterate through variables
    for (shared_ptr<Statement> statement : statementProto->getStatementVariableDeclarations()) {
        if (shared_ptr<DependencyNode> resolvedDependencyNode = dependencyNode(statement))
            resolvedDependencyNodes.push_back(resolvedDependencyNode);
    }

    return DependencyNode::dependencyNode(
        make_shared<ValueTypeProto>(
            statementProto->getSymbolName()->getGlobalName(),
            statementProto->getLocation()
        ),
        resolvedDependencyNodes
    );
}

shared_ptr<DependencyNode> ModulesStore::dependencyNode(std::shared_ptr<StatementVariableDeclaration> statementVariableDeclaration) {
    if (statementVariableDeclaration->getValueType()->isBlob())
        return DependencyNode::dependencyNode(statementVariableDeclaration->getValueType(), vector<shared_ptr<DependencyNode>>());
    
    return nullptr;
}

vector<shared_ptr<Statement>> ModulesStore::flattenedStatements(vector<shared_ptr<Statement>> statements, const string &moduleName) {
    // Check if module already visited
    for (const string &visitedModuleName : visitedModules) {
        if (moduleName == visitedModuleName)
        return {};
    }
    // Add to visited list
    visitedModules.push_back(moduleName);

    vector<shared_ptr<Statement>> resolvedFlattenedStatements;

    for (shared_ptr<Statement> statement : statements) {
        if (shared_ptr<StatementMetaImport> statementMetaImport = dynamic_pointer_cast<StatementMetaImport>(statement)) {
            if (statementMetaImport->getName() != moduleName) {
                vector<shared_ptr<Statement>> importableStatements = exportedStatementsMap[statementMetaImport->getName()];
                vector<shared_ptr<Statement>> flattenedImportableStatements = flattenedStatements(importableStatements, statementMetaImport->getName());
                resolvedFlattenedStatements.insert(resolvedFlattenedStatements.end(), flattenedImportableStatements.begin(), flattenedImportableStatements.end());
            }
        } else {
            resolvedFlattenedStatements.push_back(statement);
        }
    }

    return resolvedFlattenedStatements;
}

vector<shared_ptr<Statement>> ModulesStore::sortedBlobProtoStatements(vector<shared_ptr<DependencyNode>> dependencyNodes, vector<shared_ptr<Statement>> flattenedModuleStatements) {
    vector<shared_ptr<Statement>> resolvedSortedStatements;

    for (shared_ptr<DependencyNode> dependencyNode : dependencyNodes) {
        // Skip already visited nodes
        if (dependencyNode->getVisitStatus() == DependencyNodeVisitStatus::VISITED)
            continue;

        // Are we having a cycle?
        if (dependencyNode->getVisitStatus() == DependencyNodeVisitStatus::VISITING) {
            markErrorDependencyCycle(dependencyNode->getValueType()->getLocation());
            return resolvedSortedStatements;
        }

        dependencyNode->setVisitStatus(DependencyNodeVisitStatus::VISITING);

        // First go through each of the leafs (children)
        for (shared_ptr<DependencyNode> childDependencyNode : dependencyNode->getChildDependencyNodes()) {
            vector<shared_ptr<Statement>> resolvedChildSortedStatements = sortedBlobProtoStatements({childDependencyNode}, flattenedModuleStatements);
            resolvedSortedStatements.insert(resolvedSortedStatements.end(), resolvedChildSortedStatements.begin(), resolvedChildSortedStatements.end());
        }

        // Handle the node itself
        if (dependencyNode->getValueType() != nullptr) {
            // Find a corresponding statement
            for (shared_ptr<Statement> statement : flattenedModuleStatements) {
                // Is it a blob?
                if (dependencyNode->getValueType()->isBlob() && statement->getKind() == StatementKind::BLOB) {
                    shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(statement);
                    if (dependencyNode->getValueType()->toBlob()->getSymbolName()->isEqual(statementBlob->getSymbolName())) {
                        resolvedSortedStatements.push_back(statement);
                    }
                // Or a proto?
                } else if (dependencyNode->getValueType()->isProto() && statement->getKind() == StatementKind::PROTO) {
                    shared_ptr<StatementProto> statementProto = dynamic_pointer_cast<StatementProto>(statement);
                    if (dependencyNode->getValueType()->toProto()->getSymbolName()->isEqual(statementProto->getSymbolName())) {
                        resolvedSortedStatements.push_back(statement);
                    }
                }
            }
        }

        dependencyNode->setVisitStatus(DependencyNodeVisitStatus::VISITED);
    }

    return resolvedSortedStatements;
}

void ModulesStore::markErrorDependencyCycle(std::shared_ptr<Location> location) {
    string message = "Dependency cycle detected";
    errors.push_back(Error::error(location, message));
}

void ModulesStore::markErrorInvalidImport(shared_ptr<Location> location, const string &moduleName) {
    string message = format("Invalid import, module \"{}\" doesn't exist", moduleName);
    errors.push_back(Error::error(location, message));
}