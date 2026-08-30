#include "Analyzer.h"

#include "Error.h"
#include "Logger.h"
#include "AnalyzerScope.h"
#include "AnalyzerScopeBlob.h"
#include "Module/Module.h"
#include "Defer.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionBlock.h"
#include "Parser/Expression/ExpressionCall.h"
#include "Parser/Expression/ExpressionCast.h"
#include "Parser/Expression/ExpressionChained.h"
#include "Parser/Expression/ExpressionCompositeLiteral.h"
#include "Parser/Expression/ExpressionGrouping.h"
#include "Parser/Expression/ExpressionIfElse.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionUnary.h"
#include "Parser/Expression/ExpressionValue.h"

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

#include "Parser/ValueType/ValueType.h"
#include "Parser/ValueType/ValueTypeBoxed.h"
#include "Parser/ValueType/ValueTypeBlob.h"
#include "Parser/ValueType/ValueTypeEnum.h"
#include "Parser/ValueType/ValueTypeEnumField.h"
#include "Parser/ValueType/ValueTypeFun.h"

Analyzer::Analyzer(
    const string &defaultModuleName,
    shared_ptr<Module> module,
    const map<string, vector<shared_ptr<Statement>>> &importableHeaderStatementsMap
):
defaultModuleName(defaultModuleName),
module(module),
importableHeaderStatementsMap(importableHeaderStatementsMap) { }

void Analyzer::checkModule() {
    scope = make_shared<AnalyzerScope>();

    // check header
    for (shared_ptr<Statement> statement : module->getHeaderStatements())
        checkStatement(statement, nullptr);

    // check blob member functions
    for (shared_ptr<Statement> headerStatement : module->getHeaderStatements()) {
        if (shared_ptr<StatementBlob> statementBlob = dynamic_pointer_cast<StatementBlob>(headerStatement)) {
            for (shared_ptr<StatementFunction> statementFunction : statementBlob->getFunctionStatements()) {
                checkStatement(statementFunction);
            }
        }
    }

    // check body
    for (shared_ptr<Statement> statement : module->getBodyStatements()) {
            checkStatement(statement, nullptr);
    }

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }
}

//
// Statements
//
void Analyzer::checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType, bool isImported, ImportLevel importLevel) {
    switch (statement->getKind()) {
        case StatementKind::ASSIGNMENT: {
            checkStatement(dynamic_pointer_cast<StatementAssignment>(statement));
            break;
        }
        case StatementKind::BLOB: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementBlob>(statement), isImported);
            break;
        }
        case StatementKind::BLOB_DECLARATION: {
            checkStatement(dynamic_pointer_cast<StatementBlobDeclaration>(statement));
            break;
        }
        case StatementKind::BLOCK: {
            checkStatement(dynamic_pointer_cast<StatementBlock>(statement), returnType);
            break;
        }
        case StatementKind::ENUM: {
            checkStatement(dynamic_pointer_cast<StatementEnum>(statement));
            break;
        }
        case StatementKind::EXPRESSION: {
            checkStatement(dynamic_pointer_cast<StatementExpression>(statement), returnType);
            break;
        }
        case StatementKind::FUNCTION: {
            checkStatement(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        }
        case StatementKind::FUNCTION_DECLARATION: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementFunctionDeclaration>(statement));
            break;
        }
        case StatementKind::META_EXTERN_FUNCTION: {
            checkStatement(dynamic_pointer_cast<StatementMetaExternFunction>(statement));
            break;
        }
        case StatementKind::META_EXTERN_VARIABLE: {
            checkStatement(dynamic_pointer_cast<StatementMetaExternVariable>(statement));
            break;
        }
        case StatementKind::META_IMPORT: {
            ImportLevel newImportLevel = ImportLevel::IMPLICIT;
            if (importLevel == ImportLevel::NONE)
                newImportLevel = ImportLevel::EXPLICIT;
            checkStatement(dynamic_pointer_cast<StatementMetaImport>(statement), newImportLevel);
            break;
        }
        case StatementKind::PROTO: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementProto>(statement));
            break;
        }
        case StatementKind::PROTO_DECLARATION: {
            checkStatement(dynamic_pointer_cast<StatementProtoDeclaration>(statement));
            break;
        }
        case StatementKind::RAW_FUNCTION: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementRawFunction>(statement));
            break;
        }
        case StatementKind::REPEAT: {
            checkStatement(dynamic_pointer_cast<StatementRepeat>(statement), returnType);
            break;
        }
        case StatementKind::RETURN: {
            checkStatement(dynamic_pointer_cast<StatementReturn>(statement), returnType);
            break;
        }
        case StatementKind::VARIABLE: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementVariable>(statement));
            break;
        }
        case StatementKind::VARIABLE_DECLARATION: {
            if (importLevel != ImportLevel::IMPLICIT)
                checkStatement(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
            break;
        }
        default: {
            break;
        }
    }
}

void Analyzer::checkStatement(shared_ptr<StatementAssignment> statementAssignment) {
    shared_ptr<ValueType> targetType = typeForExpression(statementAssignment->getExpressionChained());
    if (targetType == nullptr)
        return;
    targetType = resolvedAndCheckedValueType(targetType, false, statementAssignment->getLocation());
    statementAssignment->valueExpression = checkAndTryCasting(statementAssignment->getValueExpression(), targetType, nullptr);
    if (statementAssignment->getValueExpression() == nullptr)
        return;

    shared_ptr<ValueType> sourceType = statementAssignment->getValueExpression()->getValueType();
    if (sourceType != nullptr && !sourceType->isEqual(targetType)) {
        markErrorInvalidType(
            statementAssignment->getExpressionChained()->getLocation(),
            sourceType,
            targetType
        );
    }
}

void Analyzer::checkStatement(shared_ptr<StatementBlob> statementBlob, bool isImported) {
    //scope->pushLevel();
    //scope->setNamedTypes(statementBlob->getNamedTypeKeys());
    //scope->blobScope->registerContextNamedValueTypeKeys(statementBlob->getNamedTypeKeys());
    scope->blobScope->registerNamedValueTypeKeys(statementBlob->getSymbolName(), statementBlob->getNamedTypeKeys());

    // check and verify blob member variables
    for (shared_ptr<StatementVariable> statementVariable : statementBlob->getVariableStatements()) {
        // check for invalid member names
        if (statementVariable->getIdentifier().compare("adr") == 0) {
            markErrorInvalidBuiltIn(statementVariable->getLocation(), statementVariable->getIdentifier(), statementVariable->getValueType());
            return;
        }

        // blob variable should not have a value expression
        if (statementVariable->getExpression() != nullptr) {
            markErrorUnexpectedExpression(statementVariable->getExpression()->getLocation());
            return;
        }

        // members should not have @export
        if (statementVariable->getShouldExport()) {
            markErrorInvalidAttribute(statementVariable->getLocation(), "@export");
            return;
        }
        checkStatement(statementVariable);
        if (statementVariable->getValueType() == nullptr)
            return;
    }

    // verify member functions
    for (shared_ptr<StatementFunction> statementFunction : statementBlob->getFunctionStatements()) {
        // members should not have export
        if (statementFunction->getShouldExport()) {
            markErrorInvalidAttribute(statementFunction->getLocation(), "@export");
            return;
        }
    }

    /*
    // verify proto compliance (but only if it's not an import statement)
    if (!isImported) {
        for (string &protoName : statementBlob->getProtoNames()) {
            auto protoMembers = scope->getProtoMembers(protoName);
            if (!protoMembers) {
                markErrorNotDefined(statementBlob->getLocation(), format("proto {}", protoName));
                return;
            }

            // for each proto member
            for (auto protoMember : *protoMembers) {
                bool isImplemented = false;

                if (protoMember.second->isFunction()) {
                    string name = format("{}.{}", statementBlob->getSymbolName()->getName(), protoMember.first);
                    for (shared_ptr<StatementFunction> statementFunction : statementBlob->getFunctionStatements()) {
                        // check name
                        if (name.compare(statementFunction->getName()) != 0) 
                            continue;

                        isImplemented = true;

                        // check arguments
                        //int argsCount = (*protoMember.second->getArgumentTypes()).size();

                        if (argsCount != statementFunction->getArguments().size()) {
                            isImplemented = false;
                            break;
                        }

                        for (int i=1; i<argsCount; i++) {
                            if (!(*protoMember.second->getArgumentTypes()).at(i)->isEqual(statementFunction->getArguments().at(i).second)) {
                                isImplemented = false;
                                break;
                            }
                        }

                        if (!isImplemented)
                            break;

                        // check return type
                        if (!protoMember.second->getReturnType()->isEqual(statementFunction->getReturnValueType())) {
                            isImplemented = false;
                            break;
                        }
                    }
                } else {
                    for (shared_ptr<StatementVariable> statementVariable : statementBlob->getVariableStatements()) {
                        if (protoMember.first.compare(statementVariable->getIdentifier()) == 0 && protoMember.second->isEqual(statementVariable->getValueType())) {
                            isImplemented = true;
                            break;
                        }
                    }
                }

                if (!isImplemented) {
                    markErrorNotImplemented(statementBlob->getLocation(), protoName, protoMember.first);
                    return;
                }
            }
        }
    }
    */

    // register blob members in scope
    vector<pair<string, shared_ptr<ValueType>>> members;

    // extract variable members
    for (shared_ptr<StatementVariable> statementVariable : statementBlob->getVariableStatements())
        members.push_back(pair(statementVariable->getIdentifier(), statementVariable->getValueType()));

    // then function members
    for (shared_ptr<StatementFunction> statementFunction : statementBlob->getFunctionStatements())
        members.push_back(pair(statementFunction->getName(), statementFunction->getValueType()));

    // check each of the extracted member's type
    for (auto &member : members) {
        if (resolvedAndCheckedValueType(member.second, true, statementBlob->getLocation()) == nullptr)
            return;
    }

    //scope->popLevel();

    // and the register
    //string name = importModulePrefix + statementBlob->getName();
    string name = statementBlob->getSymbolName()->getGlobalName();
    //if (!scope->setBlobMembers(name, members))
    //    markErrorAlreadyDefined(statementBlob->getLocation(), statementBlob->getSymbolName()->getGlobalName());
    scope->blobScope->registerFields(statementBlob->getSymbolName(), members);
    scope->setBlobProtoNames(name, statementBlob->getProtoNames());
}

void Analyzer::checkStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration) {
    string name = statementBlobDeclaration->getSymbolName()->getGlobalName();
    scope->blobScope->registerDeclaration(statementBlobDeclaration->getSymbolName());
}

void Analyzer::checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType) {
    for (shared_ptr<Statement> statement : statementBlock->getStatements())
        checkStatement(statement, returnType);
}

void Analyzer::checkStatement(shared_ptr<StatementEnum> statementEnum) {
    // check fields
    shared_ptr<Expression> currentTagExpression = nullptr;
    for (EnumField &field : statementEnum->fields) {
        // Only none or boxed types are valid
        if (!field.payloadValueType->isBoxed() && field.payloadValueType->getKind() != ValueTypeKind::NONE) {
            markErrorInvalidType(statementEnum->getLocation(), field.payloadValueType, nullptr);
            return;
        }

        // Make sure each field has a value expression
        if (field.tagExpression == nullptr) {
            // if none present, initialize from zero
            if (currentTagExpression == nullptr) {
                currentTagExpression = ExpressionLiteral::expressionLiteralForUInt(0, statementEnum->getLocation());
            // otherwise just add 1 to the current value
            } else {
                currentTagExpression = ExpressionBinary::expression(
                    ExpressionBinaryOperation::ADD,
                    currentTagExpression,
                    ExpressionLiteral::expressionLiteralForUInt(1, statementEnum->getLocation()),
                    statementEnum->getLocation()
                );
            }

            field.tagExpression = currentTagExpression;
        } else {
            currentTagExpression = field.tagExpression;
        }
        currentTagExpression->valueType = typeForExpression(currentTagExpression, nullptr, nullptr);

        // Register enum field type
        scope->enumScope->registerNamedValueTypeKeys(field.symbolName, statementEnum->getNamedValueTypeKeys());
        scope->enumScope->registerPayloadValueType(field.symbolName, field.payloadValueType);
    }

   // register enum type
   scope->enumScope->registerNamedValueTypeKeys(statementEnum->getSymbolName(), statementEnum->getNamedValueTypeKeys());
}

void Analyzer::checkStatement(shared_ptr<StatementExpression> statementExpression, shared_ptr<ValueType> returnType) {
    // returned value type is ignored
    statementExpression->getExpression()->valueType = typeForExpression(statementExpression->getExpression(), nullptr, returnType);
}

void Analyzer::checkStatement(shared_ptr<StatementFunction> statementFunction) {
    // check argument types
    for (pair<string, shared_ptr<ValueType>> &argument : statementFunction->getArguments()) {
        if (resolvedAndCheckedValueType(argument.second, true, statementFunction->getLocation()) == nullptr)
            return;
    }

    // update return type
    if (resolvedAndCheckedValueType(statementFunction->getReturnValueType(), true, statementFunction->getLocation()) == nullptr)
        return;

    // check if function is not yet defined and register it
    if (!scope->setFunctionType(statementFunction->getGlobalName(), statementFunction->getValueType(), true))
        markErrorAlreadyDefined(statementFunction->getLocation(), statementFunction->getName());

    scope->pushLevel();
    // register arguments as variables
    for (auto &argument : statementFunction->getArguments())
        scope->setVariableType(argument.first, argument.second, true);

    checkStatement(statementFunction->getStatementBlock(), statementFunction->getReturnValueType());
    scope->popLevel();
}

void Analyzer::checkStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration) {
    // check argument types
    for (auto &argument : statementFunctionDeclaration->getArguments()) {
        if (resolvedAndCheckedValueType(argument.second, true, statementFunctionDeclaration->getLocation()) == nullptr)
            return;
    }

    // check return type
    if (resolvedAndCheckedValueType(statementFunctionDeclaration->getReturnValueType(), true, statementFunctionDeclaration->getLocation()) == nullptr)
        return;

    string name = statementFunctionDeclaration->getGlobalName();

    if (!scope->setFunctionType(name, statementFunctionDeclaration->getValueType(), false)) {
        markErrorInvalidType(
            statementFunctionDeclaration->getLocation(),
            statementFunctionDeclaration->getValueType(),
            nullptr
        );
    }
}

void Analyzer::checkStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction) {
    // check argument types
    for (auto &argument : statementMetaExternFunction->getArguments()) {
        if (resolvedAndCheckedValueType(argument.second, true, statementMetaExternFunction->getLocation()) == nullptr)
            return;
    }

    // check return type
    if (resolvedAndCheckedValueType(statementMetaExternFunction->getReturnValueType(), true, statementMetaExternFunction->getLocation()) == nullptr)
        return;

    if (!scope->setFunctionType(statementMetaExternFunction->getGlobalName(), statementMetaExternFunction->getValueType(), false))
        markErrorAlreadyDefined(statementMetaExternFunction->getLocation(), statementMetaExternFunction->getGlobalName());
}

void Analyzer::checkStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable) {
   string identifier = statementMetaExternVariable->getGlobalIdentifier();

    if (!scope->setVariableType(identifier, statementMetaExternVariable->getValueType(), false))
        markErrorAlreadyDefined(statementMetaExternVariable->getLocation(), identifier);
}

void Analyzer::checkStatement(shared_ptr<StatementMetaImport> statementMetaImport, ImportLevel importLevel) {
    // Check if import exits
    auto it = importableHeaderStatementsMap.find(statementMetaImport->getName());
    if (it == importableHeaderStatementsMap.end()) {
        markErrorInvalidImport(statementMetaImport->getLocation(), statementMetaImport->getName());
        return;
    }

    // Skip if import circles back
    if (statementMetaImport->getName() == module->getName())
        return;
    
    // Check already imported levels
    ImportLevel currentImport = importedModuleLevelsMap[statementMetaImport->getName()];
    if (
        currentImport == ImportLevel::EXPLICIT ||
        currentImport == ImportLevel::IMPLICIT && importLevel != ImportLevel::EXPLICIT
    ) {
        return;
    }
    importedModuleLevelsMap[statementMetaImport->getName()] = importLevel;

    for (shared_ptr<Statement> &importedStatement : it->second) {
        checkStatement(importedStatement, nullptr, true, importLevel);
    }
}

void Analyzer::checkStatement(shared_ptr<StatementProto> statement) {
    scope->pushLevel();
    // check and verify proto member variables
    for (shared_ptr<StatementVariable> statementVariable : statement->getVariableStatements()) {
        // proto member variable should not have a value expression
        if (statementVariable->getExpression() != nullptr) {
            markErrorUnexpectedExpression(statementVariable->getExpression()->getLocation());
            return;
        }

        // members should not have @export
        if (statementVariable->getShouldExport()) {
            markErrorInvalidAttribute(statementVariable->getLocation(), "@export");
            return;
        }

        checkStatement(statementVariable);
    }
    scope->popLevel();

    // verify member function declarations
    for (shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration : statement->getFunctionDeclarationStatements()) {
        // members should not have export
        if (statementFunctionDeclaration->getShouldExport()) {
            markErrorInvalidAttribute(statementFunctionDeclaration->getLocation(), "@export");
            return;
        }

        checkStatement(statementFunctionDeclaration);
    }

    // register proto members in scope
    vector<pair<string, shared_ptr<ValueType>>> members;

    // extract variable members
    for (shared_ptr<StatementVariable> statementVariable : statement->getVariableStatements())
        members.push_back(pair(statementVariable->getIdentifier(), statementVariable->getValueType()));

    // then function members
    for (shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration : statement->getFunctionDeclarationStatements())
        members.push_back(pair(statementFunctionDeclaration->getName(), statementFunctionDeclaration->getValueType()));

    // check each of the extracted type
    for (auto &member : members) {
        if (resolvedAndCheckedValueType(member.second, true, statement->getLocation()) == nullptr)
            return;
    }

    // and the register
    string name = statement->getGlobalName();
    if (!scope->setProtoMembers(name, members))
        markErrorAlreadyDefined(statement->getLocation(), statement->getGlobalName());
}

void Analyzer::checkStatement(shared_ptr<StatementProtoDeclaration> statement) {
    string name = statement->getGlobalName();
    scope->setProtoMembers(name, {});
}

void Analyzer::checkStatement(shared_ptr<StatementRawFunction> statementRawFunction) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementRawFunction->getArguments())
        argumentTypes.push_back(argument.second);

    string name = statementRawFunction->getGlobalName();

    if (!scope->setFunctionType(name, statementRawFunction->getValueType(), true))
        markErrorAlreadyDefined(statementRawFunction->getLocation(), statementRawFunction->getGlobalName());
}

void Analyzer::checkStatement(shared_ptr<StatementRepeat> statementRepeat, shared_ptr<ValueType> returnType) {
    scope->pushLevel();
    if (statementRepeat->getInitStatement() != nullptr)
        checkStatement(statementRepeat->getInitStatement(), returnType);

    if (statementRepeat->getPostStatement() != nullptr)
        checkStatement(statementRepeat->getPostStatement(), returnType);

    if (shared_ptr<Expression> preConditionExpression = statementRepeat->getPreConditionExpression()) {
        preConditionExpression->valueType = typeForExpression(preConditionExpression, nullptr, nullptr);
        if (preConditionExpression->getValueType() != nullptr && !preConditionExpression->getValueType()->isEqual(ValueType::BOOL))
            markErrorInvalidType(preConditionExpression->getLocation(), preConditionExpression->getValueType(), ValueType::BOOL);
    }

    if (shared_ptr<Expression> postConditionExpression = statementRepeat->getPostConditionExpression()) {
        postConditionExpression->valueType = typeForExpression(postConditionExpression, nullptr, nullptr);
        if (postConditionExpression->getValueType() != nullptr && !postConditionExpression->getValueType()->isEqual(ValueType::BOOL))
            markErrorInvalidType(postConditionExpression->getLocation(), postConditionExpression->getValueType(), ValueType::BOOL);
    }

    // body
    checkStatement(statementRepeat->getBodyBlockStatement(), returnType);
    scope->popLevel();
}

void Analyzer::checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType) {
    statementReturn->expression = checkAndTryCasting(
        statementReturn->getExpression(),
        returnType,
        returnType
    );
    if (statementReturn->getExpression() == nullptr)
        return;

    shared_ptr<ValueType> expressionType = statementReturn->getExpression()->getValueType();

    /*if (expressionType == nullptr || !expressionType->isEqual(returnType))
        markErrorInvalidType(
            statementReturn->getLocation(),
            expressionType,
            returnType
        );
    */
}

void Analyzer::checkStatement(shared_ptr<StatementVariable> statementVariable) {
    statementVariable->valueType = resolvedAndCheckedValueType(statementVariable->getValueType(), false, statementVariable->getLocation());
    if (statementVariable->getValueType() == nullptr)
        return;

    // check initial value expression
    if (statementVariable->getExpression() != nullptr) {
        // cast expression into target
        statementVariable->expression = checkAndTryCasting(
            statementVariable->getExpression(),
            statementVariable->getValueType(),
            nullptr
        );
        if (statementVariable->getExpression() == nullptr)
            return;

        // if target has no count expression defined, use the one from source
        if (statementVariable->getValueType()->isData() && statementVariable->getValueType()->getCountExpression() == nullptr) {
            statementVariable->valueType = ValueType::data(
                statementVariable->getValueType()->getSubType(),
                statementVariable->getExpression()->getValueType()->getCountExpression()
            );
        }

        //if (!statementVariable->getValueType()->isEqual(statementVariable->getExpression()->getValueType()))
        //    markErrorInvalidType(statementVariable->getExpression()->getLocation(), statementVariable->getExpression()->getValueType(), statementVariable->getValueType());
    }

    // data types should have count expression
    if (statementVariable->getValueType()->isData() && statementVariable->getValueType()->getCountExpression() == nullptr) {
        markErrorInvalidType(statementVariable->getLocation(), statementVariable->getValueType(), nullptr);
        return;
    }

    if (!scope->setVariableType(statementVariable->getGlobalIdentifier(), statementVariable->getValueType(), true)) {
        markErrorAlreadyDefined(statementVariable->getLocation(), statementVariable->getIdentifier());
        return;
    }

    // updated corresponding variable declaration
    for (shared_ptr<Statement> headerStatement : this->module->getHeaderStatements()) {
        // find matching declaration
        shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(headerStatement);
        if (statementVariableDeclaration != nullptr && statementVariableDeclaration->getIdentifier().compare(statementVariable->getIdentifier()) == 0) {
            statementVariableDeclaration->valueType = statementVariable->getValueType();
        }
    }
}

void Analyzer::checkStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration) {
    string identifier = statementVariableDeclaration->getGlobalIdentifier();

    if (resolvedAndCheckedValueType(statementVariableDeclaration->getValueType(), true, statementVariableDeclaration->getLocation()) == nullptr)
        return;

    if (!scope->setVariableType(identifier, statementVariableDeclaration->getValueType(), false))
        markErrorAlreadyDefined(statementVariableDeclaration->getLocation(), identifier);
}

//
// Expressions
//
shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<Expression> expression, shared_ptr<Expression> parentExpression, shared_ptr<ValueType> returnType) {
    if (expression == nullptr)
        return nullptr;

    if (expression->getValueType() != nullptr && expression->getKind() != ExpressionKind::CAST)
        return expression->getValueType();

    switch (expression->getKind()) {
        case ExpressionKind::BINARY:
            return typeForExpression(dynamic_pointer_cast<ExpressionBinary>(expression));
        case ExpressionKind::BLOCK:
            return typeForExpression(dynamic_pointer_cast<ExpressionBlock>(expression), returnType);
        case ExpressionKind::CALL:
            return typeForExpression(dynamic_pointer_cast<ExpressionCall>(expression), parentExpression);
        case ExpressionKind::CAST:
            return typeForExpression(dynamic_pointer_cast<ExpressionCast>(expression), parentExpression);
        case ExpressionKind::CHAINED:
            return typeForExpression(dynamic_pointer_cast<ExpressionChained>(expression));
        case ExpressionKind::COMPOSITE_LITERAL:
            return typeForExpression(dynamic_pointer_cast<ExpressionCompositeLiteral>(expression));
        case ExpressionKind::GROUPING:
            return typeForExpression(dynamic_pointer_cast<ExpressionGrouping>(expression));
        case ExpressionKind::IF_ELSE:
            return typeForExpression(dynamic_pointer_cast<ExpressionIfElse>(expression), returnType);
        case ExpressionKind::LITERAL:
            return typeForExpression(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::NONE:
            return ValueType::NONE;
        case ExpressionKind::UNARY:
            return typeForExpression(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::VALUE:
            return typeForExpression(dynamic_pointer_cast<ExpressionValue>(expression), parentExpression);
        default:
            break;
    }
    return nullptr;
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionBinary> expressionBinary) {
    shared_ptr<ValueType> originalLeftValueType = typeForExpression(expressionBinary->getLeft(), nullptr, nullptr);
    shared_ptr<ValueType> originalRightValueType = typeForExpression(expressionBinary->getRight(), nullptr, nullptr);

    if (originalLeftValueType == nullptr || originalRightValueType == nullptr)
        return nullptr;

    // first try casting right
    // first figure out target type for right expression
    shared_ptr<ValueType> rightTargetType;
    // bit shift requires right operand to be unsigned integer
    if (expressionBinary->getOperation() == ExpressionBinaryOperation::BIT_SHL || expressionBinary->getOperation() == ExpressionBinaryOperation::BIT_SHR) {
        rightTargetType = ValueType::UINT;
    } else {
        rightTargetType = typeForExpression(expressionBinary->getLeft(), nullptr, nullptr);
    }
    if (rightTargetType == nullptr) {
        markErrorInvalidType(expressionBinary->getLeft()->getLocation(), rightTargetType, nullptr);
        return nullptr;
    }

    // try casting it
    expressionBinary->right = checkAndTryCasting(
        expressionBinary->getRight(),
        rightTargetType,
        nullptr
    );

    // and see if it suceeded
    if (expressionBinary->getRight() == nullptr)
        return nullptr;

    // then try casting left expression
    // firgure target type
    shared_ptr<ValueType> leftTargetType = typeForExpression(expressionBinary->getRight(), nullptr, nullptr);
    if (leftTargetType == nullptr) {
        markErrorInvalidType(expressionBinary->getRight()->getLocation(), leftTargetType, nullptr);
        return nullptr;
    }

    // try casting it
    expressionBinary->left = checkAndTryCasting(
        expressionBinary->getLeft(),
        leftTargetType,
        nullptr
    );

    // check if successful
    if (expressionBinary->getLeft() == nullptr)
        return nullptr;

    // validate types
    ExpressionBinaryOperation operation = expressionBinary->getOperation();
    shared_ptr<ValueType> firstType = expressionBinary->getLeft()->getValueType();
    shared_ptr<ValueType> secondType = expressionBinary->getRight()->getValueType();

    if (firstType == nullptr || secondType == nullptr)
        return nullptr;

    if (!isBinaryOperationValidForTypes(operation, firstType, secondType)) {
        markErrorInvalidOperationBinary(expressionBinary->getLocation(), operation, originalLeftValueType, originalRightValueType);
        return nullptr;
    }

    expressionBinary->valueType = typeForBinaryOperation(operation, firstType, secondType);
    return expressionBinary->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionBlock> expressionBlock, shared_ptr<ValueType> returnType) {
    checkStatement(expressionBlock->getStatementBlock(), returnType);
    checkStatement(expressionBlock->getResultStatementExpression(), returnType);
    expressionBlock->valueType = expressionBlock->getResultStatementExpression()->getExpression()->getValueType();
    return expressionBlock->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCall> expressionCall, shared_ptr<Expression> parentExpression) {
    shared_ptr<ValueType> valueType;

    int extraArguments = 0;

    Defer defer([&](){
        scope->popLevel();
    });
    scope->pushLevel();

    // check for built-in
    if (parentExpression != nullptr) {
        bool isParentPointer = parentExpression->getValueType()->isPointer();
        bool isParentBlob = parentExpression->getValueType()->isBlob();
        bool isParentProto = parentExpression->getValueType()->isProto();
        bool isVal = expressionCall->getName().compare("val") == 0;

        if (isParentPointer && isVal && parentExpression->getValueType()->getSubType()->isFunction()) {
            valueType = parentExpression->getValueType()->getSubType();
        } else if (isParentBlob) {
            shared_ptr<ValueTypeBlob> parentBlobValueType = dynamic_pointer_cast<ValueTypeBlob>(parentExpression->getValueType());
            string functionName = format("{}.{}", parentBlobValueType->getSymbolName()->getGlobalName(), expressionCall->getName());
            valueType = scope->getFunctionType(functionName);
            if (valueType == nullptr) {
                markErrorNotDefined(expressionCall->getLocation(), functionName);
                return nullptr;
            }
            extraArguments = 1; // for the implicit "it"
            scope->boxedScope->registerNamedValueTypesMap(*parentBlobValueType->getNamedValueTypeKeys(), parentBlobValueType->getNamedValueTypes());
        } else if (isParentProto) {
            string protoName = parentExpression->getValueType()->getGlobalName();
            auto members = *(scope->getProtoMembers(protoName));
            for (pair<string, shared_ptr<ValueType>> &member : members) {
                if (expressionCall->getName().compare(member.first) == 0) {
                    valueType = member.second;
                }
            }
            extraArguments = 1; // for the implicit "it"
        } else {
            markErrorInvalidType(expressionCall->getLocation(), parentExpression->getValueType()->getSubType(), nullptr);
            return nullptr;
        }
    } else {
        expressionCall->setModuleName(module->getName());
        valueType = scope->getFunctionType(expressionCall->getGlobalName());
    }

    // check if defined
    if (valueType == nullptr) {
        markErrorNotDefined(expressionCall->getLocation(), expressionCall->getGlobalName());
        return nullptr;
    }

    // check arguments count
    vector<shared_ptr<ValueType>> argumentTypes = dynamic_pointer_cast<ValueTypeFun>(valueType)->getArgumentValueTypes();
    if (argumentTypes.size() != expressionCall->getArgumentExpressions().size() + extraArguments) {
        markErrorInvalidArgumentsCount(
            expressionCall->getLocation(),
            expressionCall->getArgumentExpressions().size(),
            argumentTypes.size() - extraArguments
        );
        return nullptr;
    }
    // check argument types
    // we want to skip the implicit argumnets hence startring from "extraArguments"
    for (int i=extraArguments; i<argumentTypes.size(); i++) {
        shared_ptr<ValueType> targetType = resolvedAndCheckedValueType(argumentTypes.at(i), false, nullptr);
        /*if (parentExpression != nullptr) {
            targetType->namedTypeKeys = parentExpression->getValueType()->getNamedTypeKeys();
            targetType->namedTypeValues = parentExpression->getValueType()->getNamedTypeValues();
            targetType = resolvedAndCheckedValueType(targetType, false, parentExpression->getLocation());
        }*/

        // ignore the implicit arguments
        int argumentExpressionIndex = i - extraArguments;

        expressionCall->argumentExpressions[argumentExpressionIndex] = checkAndTryCasting(
            expressionCall->getArgumentExpressions().at(argumentExpressionIndex),
            targetType,
            dynamic_pointer_cast<ValueTypeFun>(valueType)->getReturnValueType()
        );
        if (expressionCall->getArgumentExpressions().at(argumentExpressionIndex) == nullptr)
            return nullptr;

        shared_ptr<ValueType> sourceType = expressionCall->getArgumentExpressions().at(argumentExpressionIndex)->getValueType();
        if (sourceType == nullptr)
            return nullptr;

        /*if (!sourceType->isEqual(targetType)) {
            markErrorInvalidType(
                expressionCall->getArgumentExpressions().at(argumentExpressionIndex)->getLocation(),
                sourceType,
                targetType
            );
            expressionCall->valueType = nullptr;
            return nullptr;
        }*/
    }

    expressionCall->valueType = resolvedAndCheckedValueType(dynamic_pointer_cast<ValueTypeFun>(valueType)->getReturnValueType(), false, expressionCall->getLocation());
    if (expressionCall->getValueType() == nullptr) {
        markErrorInvalidType(expressionCall->getLocation(), dynamic_pointer_cast<ValueTypeFun>(valueType)->getReturnValueType(), nullptr);
        return nullptr;
    }
    return expressionCall->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCast> expressionCast, shared_ptr<Expression> parentExpression) {
    // update count expression type
    if (expressionCast->getValueType()->getCountExpression() != nullptr) {
        expressionCast->getValueType()->getCountExpression()->valueType = typeForExpression(
            expressionCast->getValueType()->getCountExpression(),
            nullptr,
            nullptr
        );
    }

    expressionCast->valueType = resolvedAndCheckedValueType(expressionCast->getValueType(), false, expressionCast->getLocation());
    if (expressionCast->getValueType() == nullptr)
        return nullptr;

    // if the first expression in a chain is a cast, we may want to do a built-in operation on it
    if (parentExpression == nullptr)
        return expressionCast->getValueType();

    bool isSourceNumeric = parentExpression->getValueType()->isNumeric();

    bool areNumeric = parentExpression->getValueType()->isNumeric() && expressionCast->getValueType()->isNumeric();
    bool areBool = parentExpression->getValueType()->isBool() && expressionCast->getValueType()->isBool();
    bool areDataNumeric = parentExpression->getValueType()->isDataNumeric() && expressionCast->getValueType()->isDataNumeric();
    bool areDataBool = parentExpression->getValueType()->isDataBool() && expressionCast->getValueType()->isDataBool();
    bool isAddressToPointer = parentExpression->getValueType()->isAddress() && expressionCast->getValueType()->isPointer();

    bool isSourceComposite = parentExpression->getValueType()->isComposite();
    bool isSourceBoxed = parentExpression->getValueType()->isBoxed();
    bool isSourceEnum = parentExpression->getValueType()->isEnum();
    bool isTargetBlob = expressionCast->getValueType()->isBlob();
    bool isTargetData = expressionCast->getValueType()->isData();
    bool isTargetEnumField = expressionCast->getValueType()->getKind() == ValueTypeKind::ENUM_FIELD;
    bool isTargetPointer = expressionCast->getValueType()->isPointer();
    bool isTargetProto = expressionCast->getValueType()->isProto();
    bool isTargetNumeric = expressionCast->getValueType()->isNumeric();

    if (areNumeric || areBool || areDataNumeric || areDataBool | isAddressToPointer) {
        // if cast does not have a count expression, use one from the parent expression
        if (expressionCast->getValueType()->isData() && expressionCast->getValueType()->getCountExpression() == nullptr) {
            expressionCast->valueType = ValueType::data(
                expressionCast->getValueType()->getSubType(),
                parentExpression->getValueType()->getCountExpression()
            );
        }
        return expressionCast->getValueType();
    // cast composite to complex type
    } else if (isSourceComposite && (isTargetBlob || isTargetData || isTargetEnumField || isTargetPointer || isTargetProto)) {
        if (canImplicitCast(parentExpression->getValueType(), expressionCast->getValueType())) {
            // we don't want to cast the whole composite, just the individual expression
            // so we ignore the result (we don't replace the composite expression itself)
            checkAndTryCasting(parentExpression, expressionCast->getValueType(), nullptr);
            return expressionCast->getValueType();
        }
    // from boxed
    } else if (isSourceBoxed) {
        /*if (parentExpression->getValueType()->getSubType()->isEqual(expressionCast->getValueType())) {
            if (parentExpression->getValueType()->getSubType()->isPointer()) {
                expressionCast->getValueType()->getSubType()->namedTypeKeys = parentExpression->getValueType()->getSubType()->getSubType()->getNamedTypeKeys();
                expressionCast->getValueType()->getSubType()->namedTypeValues = parentExpression->getValueType()->getSubType()->getSubType()->getNamedTypeValues();
            }
            return expressionCast->getValueType();
        }*/
       return expressionCast->getValueType();
    // from enum
    } else if (isSourceEnum && isTargetNumeric) {
        return expressionCast->getValueType();
    }

    markErrorInvalidCast(expressionCast->getLocation(), parentExpression->getValueType(), expressionCast->getValueType());
    return nullptr;
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionChained> expressionChained) {
    shared_ptr<Expression> parentExpression = nullptr;

    Defer defer([&](){
        scope->popLevel();
    });
    scope->pushLevel();

    for (shared_ptr<Expression> chainExpression : expressionChained->getChainExpressions()) {
        shared_ptr<ValueType> chainType = typeForExpression(chainExpression, parentExpression, nullptr);
        chainExpression->valueType = chainType;
        parentExpression = chainExpression;
        if (chainType == nullptr)
            return nullptr;
        if (shared_ptr<ValueTypeBlob> valueTypeBlob = dynamic_pointer_cast<ValueTypeBlob>(chainType)) {
            scope->boxedScope->registerNamedValueTypesMap(*valueTypeBlob->getNamedValueTypeKeys(), valueTypeBlob->getNamedValueTypes());
        }
    }

    expressionChained->valueType = parentExpression->getValueType();
    return expressionChained->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral) {
    vector<shared_ptr<ValueType>> elementTypes;
    for (shared_ptr<Expression> expression : expressionCompositeLiteral->getExpressions()) {
        if (expression == nullptr)
            return nullptr;
        shared_ptr<ValueType> elementType = typeForExpression(expression, nullptr, nullptr);
        if (elementType == nullptr)
            return nullptr;
        elementTypes.push_back(elementType);
    }
    shared_ptr<Expression> countExpression = ExpressionLiteral::expressionLiteralForUInt(elementTypes.size(), expressionCompositeLiteral->getLocation());
    countExpression->valueType = typeForExpression(countExpression, nullptr, nullptr);
    expressionCompositeLiteral->valueType = ValueType::composite(elementTypes, countExpression);
    return expressionCompositeLiteral->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionGrouping> expressionGrouping) {
    expressionGrouping->valueType = typeForExpression(expressionGrouping->getSubExpression(), nullptr, nullptr);
    return expressionGrouping->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionIfElse> expressionIfElse, shared_ptr<ValueType> returnType) {
    // first check that condition is as BOOL
    expressionIfElse->conditionExpression = checkAndTryCasting(expressionIfElse->getConditionExpression(), ValueType::BOOL, returnType);
    if (expressionIfElse->getConditionExpression() == nullptr)
        return nullptr;
    shared_ptr<ValueType> conditionType = expressionIfElse->getConditionExpression()->getValueType();
    if (conditionType == nullptr) {
        return nullptr;
    } else if (!conditionType->isEqual(ValueType::BOOL)) {
        markErrorInvalidType(
            expressionIfElse->getConditionExpression()->getLocation(),
            conditionType, ValueType::BOOL
        );
    }

    // try cross-casting if else is present
    if (expressionIfElse->getElseExpression() != nullptr) {
        scope->pushLevel();
        shared_ptr<ValueType> elseType = typeForExpression(expressionIfElse->getElseExpression(), nullptr, returnType);
        scope->popLevel();
        if (elseType == nullptr)
            return nullptr;

        scope->pushLevel();
        expressionIfElse->thenExpression = checkAndTryCasting(
            expressionIfElse->getThenExpression(),
            elseType,
            returnType
        );
        scope->popLevel();

        if (expressionIfElse->getThenExpression() == nullptr)
            return nullptr;

        if (expressionIfElse->getThenExpression()->getValueType() == nullptr)
            return nullptr;

        scope->pushLevel();
        expressionIfElse->elseExpression = checkAndTryCasting(
            expressionIfElse->getElseExpression(),
            expressionIfElse->getThenExpression()->getValueType(),
            returnType
        );
        scope->popLevel();
    // otherwise just register then block's type
    } else {
        scope->pushLevel();
        expressionIfElse->getThenExpression()->valueType = typeForExpression(
            expressionIfElse->getThenExpression(),
            nullptr,
            returnType
        );
        scope->popLevel();
        if (expressionIfElse->getThenExpression()->getValueType() == nullptr)
            return nullptr;
    }

    // finally, figure out resulting type
    shared_ptr<ValueType> thenType = expressionIfElse->getThenExpression()->getValueType();
    shared_ptr<ValueType> elseType = expressionIfElse->getElseExpression() != nullptr ? expressionIfElse->getElseExpression()->getValueType() : nullptr;
    expressionIfElse->valueType = thenType->isEqual(elseType) ? thenType : ValueType::NONE;

    return expressionIfElse->getValueType();
}

shared_ptr<ValueType> Analyzer::Analyzer::typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral) {
    // if it's already set, return it
    if (expressionLiteral->getValueType() != nullptr)
        return expressionLiteral->getValueType();

    // otherwise get a default one
    switch (expressionLiteral->getLiteralKind()) {
        case ExpressionLiteralKind::BOOL:
            expressionLiteral->valueType = ValueType::BOOL;
            break;
        case ExpressionLiteralKind::UINT:
            expressionLiteral->valueType = ValueType::UINT;
            break;
        case ExpressionLiteralKind::FLOAT:
            expressionLiteral->valueType = ValueType::FLOAT;
            break;
        default:
            markErrorInvalidType(expressionLiteral->getLocation(), nullptr, nullptr);
    }

    return expressionLiteral->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    ExpressionUnaryOperation operation = expressionUnary->getOperation();
    shared_ptr<ValueType> subType = typeForExpression(expressionUnary->getSubExpression(), nullptr, nullptr);
    if (subType == nullptr)
        return nullptr;

    if (!isUnaryOperationValidForType(expressionUnary->getOperation(), subType)) {
        markErrorInvalidOperationUnary(expressionUnary->getLocation(), operation, subType);
        return nullptr;
    }
    
    expressionUnary->valueType = typeForUnaryOperation(operation, subType);

    return expressionUnary->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionValue> expressionValue, shared_ptr<Expression> parentExpression) {
    if (parentExpression != nullptr) {
        // check built-in
        bool isParentData = parentExpression->getValueType()->isData();
        bool isParentPointer = parentExpression->getValueType()->isPointer();
        bool isParentBlob = parentExpression->getValueType()->isBlob();
        bool isParentProto = parentExpression->getValueType()->isProto();

        bool isCount = expressionValue->getIdentifier().compare("count") == 0;
        bool isVal = expressionValue->getIdentifier().compare("val") == 0;
        bool isVadr = expressionValue->getIdentifier().compare("vadr") == 0;
        bool isAdr = expressionValue->getIdentifier().compare("adr") == 0;
        bool isSize = expressionValue->getIdentifier().compare("size") == 0;

        if (isParentData && isCount) {
            expressionValue->valueType = ValueType::UINT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_COUNT;
            return expressionValue->getValueType();
        } else if (isParentPointer && isVal) {
            switch (expressionValue->getValueKind()) {
                case ExpressionValueKind::SIMPLE:
                case ExpressionValueKind::BUILT_IN_VAL_SIMPLE:
                    expressionValue->valueType = parentExpression->getValueType()->getSubType();
                    expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_SIMPLE;
                    break;
                case ExpressionValueKind::DATA:
                case ExpressionValueKind::BUILT_IN_VAL_DATA:
                    // make sure we're referencing a pointer to data
                    if (!parentExpression->getValueType()->getSubType()->isData()) {
                        expressionValue->valueType = nullptr;
                        markErrorInvalidBuiltIn(
                            expressionValue->getLocation(),
                            expressionValue->getIdentifier(),
                            parentExpression->getValueType()
                        );
                        break;
                    }
                    expressionValue->valueType = parentExpression->getValueType()->getSubType()->getSubType();
                    expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_DATA;
                    expressionValue->indexExpression = checkAndTryCasting(expressionValue->getIndexExpression(), ValueType::UINT, nullptr);
                    break;
                default:
                    expressionValue->valueType = nullptr;
                    markErrorInvalidBuiltIn(
                        expressionValue->getLocation(),
                        expressionValue->getIdentifier(),
                        parentExpression->getValueType()
                    );
                    break;
            }
            return expressionValue->getValueType();
        } else if ((isParentPointer || isParentProto) && isVadr) {
            expressionValue->valueType = ValueType::A;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VADR;
            return expressionValue->getValueType();
        } else if (isAdr) {
            expressionValue->valueType = ValueType::A;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_ADR;
            return expressionValue->getValueType();
        } else if (isSize) {
            expressionValue->valueType = ValueType::UINT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_SIZE;
            return expressionValue->getValueType();
        // check blob member
        } else if (isParentBlob) {
            shared_ptr<ValueTypeBlob> blobValueType = dynamic_pointer_cast<ValueTypeBlob>(parentExpression->getValueType());
            string blobName = blobValueType->getSymbolName()->getGlobalName();
            optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = scope->blobScope->getFields(blobValueType->getSymbolName());
            if (blobMembers) {
                scope->pushLevel();
                scope->boxedScope->registerNamedValueTypesMap(*blobValueType->getNamedValueTypeKeys(), blobValueType->getNamedValueTypes());

                string nameVariable = expressionValue->getIdentifier();
                string nameFunction = format("{}.{}", blobName, expressionValue->getIdentifier());
                for (pair<string, shared_ptr<ValueType>> &blobMember : *blobMembers) {
                    if (nameVariable.compare(blobMember.first) == 0 || nameFunction.compare(blobMember.first) == 0) {
                        // found corresponding blob, decide if it's a simple or data access
                        switch (expressionValue->getValueKind()) {
                            case ExpressionValueKind::SIMPLE: {
                                // resolve type of named type if required
                                expressionValue->valueType = resolvedAndCheckedValueType(blobMember.second, false, expressionValue->getLocation());
                                scope->popLevel();
                                return expressionValue->getValueType();
                            }
                            case ExpressionValueKind::DATA: {
                                // make sure that the indexed value is an array
                                shared_ptr<ValueType> valueType = blobMember.second;
                                if (valueType->getKind() != ValueTypeKind::DATA) {
                                    markErrorInvalidType(expressionValue->getLocation(), valueType, nullptr);
                                    return nullptr;
                                }
                                expressionValue->valueType = blobMember.second->getSubType();
                                expressionValue->getIndexExpression()->valueType = typeForExpression(expressionValue->getIndexExpression(), nullptr, nullptr);
                                // make sure that the index expression evaluates to an uint
                                shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
                                if (!indexExpression->getValueType()->isUnsignedInteger()) {
                                    markErrorInvalidType(indexExpression->getLocation(), indexExpression->getValueType(), ValueType::UINT);
                                    scope->popLevel();
                                    return nullptr;
                                }
                                scope->popLevel();
                                return expressionValue->getValueType();
                            }
                            default:
                                break;
                        }
                    }
                }
                scope->popLevel();
            }
            markErrorNotDefined(
                expressionValue->getLocation(),
                format("{}.{}", blobName, expressionValue->getIdentifier())
            );
            return nullptr;
        // check proto member
        } else if (isParentProto) {
            string protoName = parentExpression->getValueType()->getGlobalName();
            auto members = *(scope->getProtoMembers(protoName));
            for (pair<string, shared_ptr<ValueType>> &member : members) {
                if (expressionValue->getIdentifier() == member.first) {
                    // found corresponding member, decide if it's a simple or data access
                    switch (expressionValue->getValueKind()) {
                        case ExpressionValueKind::SIMPLE:
                            expressionValue->valueType = member.second;
                            return expressionValue->getValueType();
                        case ExpressionValueKind::DATA: {
                            // make sure that the indexed value is an array
                            shared_ptr<ValueType> valueType = member.second;
                            if (valueType->getKind() != ValueTypeKind::DATA) {
                                markErrorInvalidType(expressionValue->getLocation(), valueType, nullptr);
                                return nullptr;
                            }
                            expressionValue->valueType = member.second->getSubType();
                            expressionValue->getIndexExpression()->valueType = typeForExpression(expressionValue->getIndexExpression(), nullptr, nullptr);
                            // make sure that the index expression evaluates to an uint
                            shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
                            if (!indexExpression->getValueType()->isUnsignedInteger()) {
                                markErrorInvalidType(indexExpression->getLocation(), indexExpression->getValueType(), ValueType::UINT);
                                return nullptr;
                            }
                            return expressionValue->getValueType();
                        }
                        default:
                            break;
                    }
                }
            }
            markErrorNotDefined(
                expressionValue->getLocation(),
                format("{}.{}", protoName, expressionValue->getIdentifier())
            );
            return nullptr;
        }
    }

    // first assume it's just simple
    shared_ptr<ValueType> type = scope->getVariableType(expressionValue->getIdentifier());
    // try with global identifier
    if (type == nullptr && expressionValue->getModuleName().empty()) {
        string globalIdentifier = format("{}.{}", module->getName(), expressionValue->getIdentifier());
        type = scope->getVariableType(globalIdentifier);
        if (type != nullptr)
            expressionValue->setModuleName(module->getName());
    }
    if (type != nullptr) {
        expressionValue->valueKind = ExpressionValueKind::SIMPLE;
    }

    // then check if it's data
    if (type != nullptr && expressionValue->getIndexExpression() != nullptr) {
        // make sure that the indexed value is an array
        if (type->getKind() != ValueTypeKind::DATA) {
            markErrorInvalidType(expressionValue->getLocation(), type, nullptr);
            return nullptr;
        }
        expressionValue->indexExpression = checkAndTryCasting(
            expressionValue->getIndexExpression(),
            ValueType::UINT,
            nullptr
        );
        shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
        // make sure that the index expression evaluates to an uint
        if (!indexExpression->getValueType()->isUnsignedInteger()) {
            markErrorInvalidType(indexExpression->getLocation(), indexExpression->getValueType(), ValueType::UINT);
            return nullptr;
        }
        type = type->getSubType();
        expressionValue->valueKind = ExpressionValueKind::DATA;
    // check if it's blob's `it`
    } else if (type == nullptr && expressionValue->getIdentifier() == "it") {
        shared_ptr<ValueType> blobPtrType = scope->getVariableType(".pit");
        if (blobPtrType == nullptr) {
            markErrorNotDefined(expressionValue->getLocation(), expressionValue->getIdentifier());
            return nullptr;
        }
        type = blobPtrType->getSubType();
        expressionValue->valueKind = ExpressionValueKind::SIMPLE;
    // check if it's a function
    } else if (type == nullptr) {
        type = scope->getFunctionType(expressionValue->getIdentifier());
        if (type == nullptr && expressionValue->getModuleName().empty()) {
            string globalIdentifier = format("{}.{}", module->getName(), expressionValue->getIdentifier());
            type = scope->getFunctionType(globalIdentifier);
            if (type != nullptr)
                expressionValue->setModuleName(module->getName());
        }
        if (type != nullptr)
            expressionValue->valueKind = ExpressionValueKind::FUN;
    }
    
    if (type == nullptr && parentExpression != nullptr) {
        markErrorNotDefined(expressionValue->getLocation(), format("member \".{}\"", expressionValue->getIdentifier()));
    } else if (type == nullptr) {
        markErrorNotDefined(expressionValue->getLocation(), format("\"{}\"", expressionValue->getIdentifier()));
    }

    expressionValue->valueType = type;
    return expressionValue->getValueType();
}

//
// Support
//
bool Analyzer::isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) const {
    switch (type->getKind()) {
        // bool
        case ValueTypeKind::BOOL:
            switch (operation) {
                case ExpressionUnaryOperation::NOT: {
                    return true;
                }
                default:
                    break;
            break;
        }
        // numeric
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64: {
            switch (operation) {
                case ExpressionUnaryOperation::BIT_NOT:
                case ExpressionUnaryOperation::PLUS:
                case ExpressionUnaryOperation::MINUS: {
                    return true;
                }
                default:
                    break;
            }
            break;
        }

        // address
        case ValueTypeKind::A: {
            switch (operation) {
                case ExpressionUnaryOperation::BIT_NOT:
                case ExpressionUnaryOperation::PLUS: {
                    return true;
                }
                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    // all other combinations are invalid
    return false;
}

bool Analyzer::isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) const{
    // Unbox types if required
    if (firstType->isBoxed())
        firstType = firstType->getSubType();

    if (secondType->isBoxed())
        secondType = secondType->getSubType();

    switch (firstType->getKind()) {
        // Valid operations for boolean types
        case ValueTypeKind::BOOL: {
            switch (operation) {
                case ExpressionBinaryOperation::EQUAL:
                case ExpressionBinaryOperation::NOT_EQUAL:

                case ExpressionBinaryOperation::OR:
                case ExpressionBinaryOperation::XOR:
                case ExpressionBinaryOperation::AND:
                    return firstType->isEqual(secondType);
                default:
                    break;
            }
            break;
        }
        // Valid operations for numeric types
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64:
        
        case ValueTypeKind::A: {
            switch (operation) {
                // shift operations requires second type to be an unsigned integer
                case ExpressionBinaryOperation::BIT_SHL:
                case ExpressionBinaryOperation::BIT_SHR: {
                    switch (secondType->getKind()) {
                        case ValueTypeKind::UINT:
                        case ValueTypeKind::U8:
                        case ValueTypeKind::U16:
                        case ValueTypeKind::U32:
                        case ValueTypeKind::U64: {
                            return true;
                        }
                        default:
                            break;
                    }
                    break;
                }

                // other operations have to match
                case ExpressionBinaryOperation::BIT_TEST:
                case ExpressionBinaryOperation::BIT_OR:
                case ExpressionBinaryOperation::BIT_XOR:
                case ExpressionBinaryOperation::BIT_AND:

                case ExpressionBinaryOperation::EQUAL:
                case ExpressionBinaryOperation::NOT_EQUAL:
                case ExpressionBinaryOperation::LESS:
                case ExpressionBinaryOperation::LESS_EQUAL:
                case ExpressionBinaryOperation::GREATER:
                case ExpressionBinaryOperation::GREATER_EQUAL:

                case ExpressionBinaryOperation::ADD:
                case ExpressionBinaryOperation::SUB:
                case ExpressionBinaryOperation::MUL:
                case ExpressionBinaryOperation::DIV:
                case ExpressionBinaryOperation::MOD: {
                    return firstType->isEqual(secondType);
                }
                default:
                    break;
            }
            break;
        }
        case ValueTypeKind::ENUM: {
            return firstType->isEqual(secondType);
        }
        default:
            break;
    }

    // all other combinations are invalid
    return false;
}

shared_ptr<ValueType> Analyzer::typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) const {
    switch (operation) {
        case ExpressionUnaryOperation::MINUS:
            switch (type->getKind()) {
                case ValueTypeKind::UINT:
                    return ValueType::SINT;
                case ValueTypeKind::U8:
                    return ValueType::S8;
                case ValueTypeKind::U16:
                    return ValueType::S16;
                case ValueTypeKind::U32:
                    return ValueType::S32;
                case ValueTypeKind::U64:
                    return ValueType::S64;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    // no change
    return type;
}

 shared_ptr<ValueType> Analyzer::typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) const {
    switch (operation) {
        case ExpressionBinaryOperation::EQUAL:
        case ExpressionBinaryOperation::NOT_EQUAL:
        case ExpressionBinaryOperation::LESS:
        case ExpressionBinaryOperation::LESS_EQUAL:
        case ExpressionBinaryOperation::GREATER:
        case ExpressionBinaryOperation::GREATER_EQUAL:
        case ExpressionBinaryOperation::BIT_TEST:
            return ValueType::BOOL;
        default:
            break;
    }

    // not change
    return firstType;
}

shared_ptr<Expression> Analyzer::checkAndTryCasting(shared_ptr<Expression> sourceExpression, shared_ptr<ValueType> targetType, shared_ptr<ValueType> returnType) {
    if (sourceExpression == nullptr)
        return nullptr;

    shared_ptr<ValueType> sourceType = typeForExpression(sourceExpression, nullptr, returnType);
    if (sourceType == nullptr)
        return nullptr;
    sourceExpression->valueType = sourceType;
    if (sourceType->isEqual(targetType))
        return sourceExpression;

    if (!canImplicitCast(sourceType, targetType))
        return sourceExpression;

    // single literal just needs to set the type
    if (sourceExpression->getKind() == ExpressionKind::LITERAL && !targetType->isBoxed()) {
        sourceExpression->valueType = targetType;
        return sourceExpression;
    // composite to blob
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isBlob()) {
        shared_ptr<ValueTypeBlob> targetValueTypeBlob = dynamic_pointer_cast<ValueTypeBlob>(targetType);
        scope->pushLevel();
        scope->boxedScope->registerNamedValueTypesMap(*targetValueTypeBlob->getNamedValueTypeKeys(), targetValueTypeBlob->getNamedValueTypes());

        sourceExpression->valueType = targetType;
        vector<shared_ptr<ValueType>> blobMemberTypes = *scope->blobScope->getVariableFieldValueTypes(dynamic_pointer_cast<ValueTypeBlob>(targetType)->getSymbolName());
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        for (int i=0; i<blobMemberTypes.size(); i++) {
            shared_ptr<ValueType> memberType = blobMemberTypes.at(i);
            expressionCompositeLiteral->expressions[i] = checkAndTryCasting(expressionCompositeLiteral->getExpressions().at(i), memberType, returnType);
        }

        scope->popLevel();

        return sourceExpression;
    // composite to proto
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isProto()) {
        sourceExpression->valueType = targetType;
        return sourceExpression;
    // composite to data
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isData()) {
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        // first update the type
        sourceExpression->valueType = ValueType::data(
            targetType->getSubType(),
            ExpressionLiteral::expressionLiteralForUInt(
                expressionCompositeLiteral->getExpressions().size(),
                sourceExpression->getLocation()
            )
        );
        sourceExpression->getValueType()->getCountExpression()->valueType = typeForExpression(sourceExpression->getValueType()->getCountExpression(), nullptr, returnType);
        // and then cast (if necessary) each of the element expressions
        for (int i=0; i<expressionCompositeLiteral->getExpressions().size(); i++) {
            shared_ptr<Expression> sourceElementExpression = expressionCompositeLiteral->getExpressions().at(i);
            sourceElementExpression = checkAndTryCasting(sourceElementExpression, targetType->getSubType(), returnType);
        }
        // check if types are already equal or we need additional cast
        if (targetType->getCountExpression() == nullptr || expressionCompositeLiteral->getValueType()->isEqual(targetType))
            return sourceExpression;
    // composite to pointer
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isPointer()) {
        sourceExpression->valueType = targetType;
        // make sure the composite element expression is of type a
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        shared_ptr<Expression> sourceElementExpression = expressionCompositeLiteral->getExpressions().at(0);
        sourceElementExpression = checkAndTryCasting(sourceElementExpression, ValueType::A, nullptr);
        return sourceExpression;
    // data to data
    } else if (sourceExpression->getValueType()->isData() && targetType->isData()) {
        if (sourceType->getCountExpression() != nullptr)
            sourceType->getCountExpression()->valueType = typeForExpression(sourceType->getCountExpression(), nullptr, returnType);

        if (targetType->getCountExpression() == nullptr)
            return sourceExpression;
    } else if (sourceExpression->getKind() == ExpressionKind::IF_ELSE) {
        sourceExpression->valueType = targetType;
        shared_ptr<ExpressionIfElse> expressionIfElse = dynamic_pointer_cast<ExpressionIfElse>(sourceExpression);
        expressionIfElse->thenExpression = checkAndTryCasting(expressionIfElse->getThenExpression(), targetType, returnType);
        expressionIfElse->elseExpression = checkAndTryCasting(expressionIfElse->getElseExpression(), targetType, returnType);
        return sourceExpression;
    } else if (sourceExpression->getKind() == ExpressionKind::BLOCK) {
        sourceExpression->valueType = targetType;
        shared_ptr<ExpressionBlock> expressionBlock = dynamic_pointer_cast<ExpressionBlock>(sourceExpression);
        expressionBlock->getResultStatementExpression()->expression = checkAndTryCasting(expressionBlock->getResultStatementExpression()->getExpression(), targetType, returnType);
        return expressionBlock;
    }

    // if target has no count expression defined, use the one from source
    if (targetType->isData() && targetType->getCountExpression() == nullptr) {
        targetType = ValueType::data(
            targetType->getSubType(),
            sourceExpression->getValueType()->getCountExpression()
        );
    }

    // create target cast
    shared_ptr<ExpressionChained> targetExpression;

    if (targetType->isBoxed()) {
        targetType = resolvedAndCheckedValueType(targetType, false, sourceExpression->getLocation());
        if (targetType == nullptr)
            return nullptr;

        shared_ptr<ValueTypeBoxed> valueTypeBoxed = dynamic_pointer_cast<ValueTypeBoxed>(targetType);

        sourceExpression = checkAndTryCasting(sourceExpression, valueTypeBoxed->getBoxedValueType(), returnType);

        targetExpression = make_shared<ExpressionChained>(
            vector<shared_ptr<Expression>>(
                {
                    sourceExpression,
                    make_shared<ExpressionCast>(valueTypeBoxed, sourceExpression->getLocation())
                }
            ),
            sourceExpression->getLocation()
        );

        targetExpression->valueType = valueTypeBoxed;
    } else {
        targetExpression = make_shared<ExpressionChained>(
            vector<shared_ptr<Expression>>(
                {
                    sourceExpression,
                    make_shared<ExpressionCast>(targetType, sourceExpression->getLocation())
                }
            ),
            sourceExpression->getLocation()
        );

        targetExpression->valueType = targetType;
    }
    return targetExpression;
}

bool Analyzer::canImplicitCast(shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    resolvedAndCheckedValueType(targetType, false, nullptr);

    switch (sourceType->getKind()) {
        // from literal types
        case ValueTypeKind::UINT: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U8:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;   
            }
            break;
        }

        case ValueTypeKind::SINT: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;   
            }
            break;
        }

        case ValueTypeKind::FLOAT: {
            switch (targetType->getKind()) {
                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;   
            }
            break;
        }

        // from unsigned
        case ValueTypeKind::U8: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U8:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::U16: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::U32: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;
                
                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::U64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::UINT:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }

        // from signed
        case ValueTypeKind::S8: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::S16: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::S32: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::S64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::SINT:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }

        // from float
        case ValueTypeKind::F32: {
            switch (targetType->getKind()) {
                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }
        case ValueTypeKind::F64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F64:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }

        // from address
        case ValueTypeKind::A: {
            switch (targetType->getKind()) {
                case ValueTypeKind::A:
                    return true;

                case ValueTypeKind::BOXED:
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());

                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/

                default:
                    return false;
            }
            break;
        }

        // from pointer
        case ValueTypeKind::PTR: {
            switch (targetType->getKind()) {
                case ValueTypeKind::PTR: {
                    return sourceType->getSubType()->isEqual(targetType->getSubType());
                }
                case ValueTypeKind::BOXED: {
                    return canImplicitCast(sourceType, dynamic_pointer_cast<ValueTypeBoxed>(targetType)->getBoxedValueType());
                }
                /*case ValueTypeKind::NAMED_TYPE: {
                    shared_ptr<ValueType> resolvedNamedValueType = resolvedAndCheckedValueType(targetType, false, nullptr);
                    if (resolvedNamedValueType->isNamedType())
                        return false;
                    return canImplicitCast(sourceType, resolvedNamedValueType);
                }*/
                default:
                    return false;
            }
            break;
        }

        // from data
        case ValueTypeKind::DATA: {
            switch (targetType->getKind()) {
                case ValueTypeKind::DATA:
                    return canImplicitCast(sourceType->getSubType(), targetType->getSubType());

                default:
                    return false;
            }
            break;
        }

        // blob
        case ValueTypeKind::BLOB: {
            if (!targetType->isBlob())
                return false;

            string sourceBlobName = sourceType->getGlobalName();
            string targetBlobName = targetType->getGlobalName();

            return sourceBlobName.compare(targetBlobName) == 0;
        }

        // from composite
        case ValueTypeKind::COMPOSITE: {
            switch (targetType->getKind()) {
                // to pointer
                case ValueTypeKind::PTR: {
                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());
                    return sourceElementTypes.size() == 1 && sourceElementTypes.at(0)->isInteger();
                }

                // to data
                case ValueTypeKind::DATA: {
                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());
                    for (shared_ptr<ValueType> sourceElementType : sourceElementTypes) {
                        if (!canImplicitCast(sourceElementType, targetType->getSubType()))
                            return false;
                    }
                    return true;
                }

                // to blob
                case ValueTypeKind::BLOB: {
                    shared_ptr<ValueTypeBlob> targetValueTypeBlob = dynamic_pointer_cast<ValueTypeBlob>(targetType);
                    // get target non-function types
                    //optional<vector<shared_ptr<ValueType>>> targetMemberTypes = scope->getNonFunctionBlobMemberTypes(targetType);
                    optional<vector<shared_ptr<ValueType>>> oTargetFieldValueTypes = scope->blobScope->getVariableFieldValueTypes(targetValueTypeBlob->getSymbolName());
                    if (!oTargetFieldValueTypes)
                        return false;

                    // get source types
                    vector<shared_ptr<ValueType>> sourceElementTypes = *sourceType->getCompositeElementTypes();

                    // check that number of memebrs match
                    if (sourceElementTypes.size() != (*oTargetFieldValueTypes).size())
                        return false;

                    // check that each entry in composite can be cast to member in blob
                    scope->pushLevel();
                    scope->boxedScope->registerNamedValueTypesMap(*targetValueTypeBlob->getNamedValueTypeKeys(), targetValueTypeBlob->getNamedValueTypes());
                    for (int i=0; i<(*oTargetFieldValueTypes).size(); i++) {
                        if (!canImplicitCast(sourceElementTypes.at(i), (*oTargetFieldValueTypes).at(i)))
                            return false;
                    }
                    scope->popLevel();
                    return true;
                }

                // to enum field
                case ValueTypeKind::ENUM_FIELD: {
                    // get target & source types
                    shared_ptr<ValueTypeEnumField> targetValueTypeEnumField = dynamic_pointer_cast<ValueTypeEnumField>(targetType);
                    vector<shared_ptr<ValueType>> sourceElementTypes = *sourceType->getCompositeElementTypes();

                    // first option, no source types and target is none
                    if (sourceElementTypes.size() == 0) {
                        return targetValueTypeEnumField->getPayloadValueType()->getKind() == ValueTypeKind::NONE;
                    }

                    // otherwise source has to have one element
                    if (sourceElementTypes.size() != 1)
                        return false;

                    // and check if it can be cast
                    return canImplicitCast(sourceElementTypes.front(), targetValueTypeEnumField->getPayloadValueType());
                }

                // to proto
                case ValueTypeKind::PROTO: {
                    string targetProtoName = targetType->getGlobalName();

                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());
                    if (sourceElementTypes.size() != 1 || !sourceElementTypes.at(0)->isPointer())
                        return false;

                    shared_ptr<ValueType> subType = sourceElementTypes.at(0)->getSubType();
                    if (subType == nullptr || !subType->isBlob())
                        return false;

                    string blobName = dynamic_pointer_cast<ValueTypeBlob>(subType)->getSymbolName()->getGlobalName();
                    optional<vector<string>> protoNames = scope->getBlobProtoNames(blobName);
                    if (!protoNames)
                        return false;
                    
                    for (string &protoName : *protoNames) {
                        if (targetProtoName.compare(protoName) == 0)
                            return true;
                    }

                    return false;
                }

                default:
                    return false;
            }
            break;
        }

        // from boxed
        case ValueTypeKind::BOXED: {
            switch (targetType->getKind()) {
                // TODO: Having automatic unboxing cast would be nice
                /*
                case ValueTypeKind::UINT:
                case ValueTypeKind::U8:
                case ValueTypeKind::U16:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::SINT:
                case ValueTypeKind::S8:
                case ValueTypeKind::S16:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::FLOAT:
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:

                case ValueTypeKind::A:
                case ValueTypeKind::PTR:
                    return canCast(sourceType->getSubType(), targetType);
                */

                default:
                    return false;
            }
        }

        // from enum field
        case ValueTypeKind::ENUM_FIELD: {
            // Are identical?
            if (sourceType->isEqual(targetType))
                return true;
            
            // Is this a field of a parent enum?
            shared_ptr<ValueTypeEnumField> sourceValueTypeEnumField = dynamic_pointer_cast<ValueTypeEnumField>(sourceType);
            shared_ptr<ValueTypeEnum> targetValueTypeEnum = dynamic_pointer_cast<ValueTypeEnum>(targetType);
            if (targetValueTypeEnum == nullptr)
                return false;

            if (!sourceValueTypeEnumField->getSymbolName()->getGlobalName().starts_with(targetValueTypeEnum->getSymbolName()->getGlobalName()))
                return false;

            // Does number of named types match?
            if (sourceValueTypeEnumField->getNamedValueTypes().size() != targetValueTypeEnum->getNamedValueTypes().size())
                return false;

            // Are the named value types identical?
            for (int i=0; i<sourceValueTypeEnumField->getNamedValueTypes().size(); i++) {
                shared_ptr<ValueType> sourceNamedValueType = sourceValueTypeEnumField->getNamedValueTypes().at(i);
                shared_ptr<ValueType> targetNamedValueType = targetValueTypeEnum->getNamedValueTypes().at(i);
                if (!sourceNamedValueType->isEqual(targetNamedValueType))
                    return false;
            }

            return true;
        }

        default:
            return false;
    }
}

shared_ptr<ValueType> Analyzer::resolvedAndCheckedValueType(shared_ptr<ValueType> valueType, bool isCountExperssionRequired, shared_ptr<Location> location) {
    switch (valueType->getKind()) {
        case ValueTypeKind::BLOB: {
            // TODO: This gets messed up because of imported sub-sub-modules
            /*
            if (!scope->isBlobDeclared(*valueType->getBlobName())) {
                markErrorInvalidType(location, valueType, nullptr);
                return false;
            } else
            */
            /*
            if (!valueType->namedTypeKeys)
                valueType->namedTypeKeys = scope->getBlobNamedTypeKeys(valueType->getGlobalName());
            return valueType;
            */
           return checkValueType(dynamic_pointer_cast<ValueTypeBlob>(valueType));
        }
        case ValueTypeKind::BOXED: {
            return checkValueType(dynamic_pointer_cast<ValueTypeBoxed>(valueType));
        }
        case ValueTypeKind::DATA: {
            if (valueType->getCountExpression() != nullptr) {
                valueType->getCountExpression()->valueType = typeForExpression(valueType->getCountExpression(), nullptr, nullptr);
                return valueType;
            } else if (isCountExperssionRequired) {
                markErrorInvalidType(location, valueType, nullptr);
                return nullptr;
            }
            return valueType;
        }
        case ValueTypeKind::ENUM: {
            return checkValueType(dynamic_pointer_cast<ValueTypeEnum>(valueType));
        }
        case ValueTypeKind::ENUM_FIELD: {
            return checkValueType(dynamic_pointer_cast<ValueTypeEnumField>(valueType));
        }
        case ValueTypeKind::FUN: {
            return checkValueType(dynamic_pointer_cast<ValueTypeFun>(valueType));
        }
        case ValueTypeKind::PTR: {
            return ValueType::ptr(resolvedAndCheckedValueType(valueType->getSubType(), false, location), valueType->getIsVolatile());
        }
        default: {
            return valueType;
        }
    }
}

shared_ptr<ValueType> Analyzer::checkValueType(shared_ptr<ValueTypeBlob> valueTypeBlob) {    
    // Check if blob is registered
    if (scope->blobScope->getState(valueTypeBlob->getSymbolName()) == AnalyzerScopeState::NOT_REGISTERED) {
        markErrorNotDefined(nullptr, valueTypeBlob->getSymbolName()->getGlobalName());
        return nullptr;
    }
    
    /*
    // check 
    scope->pushLevel();
    scope->boxedScope->registerNamedValueTypesMap(*oNamedValueTypeKeys, valueTypeBlob->getNamedValueTypes());
    scope->popLevel();
    */
    optional<vector<string>> oNamedValueTypeKeys = scope->blobScope->getNamedValueTypeKeys(valueTypeBlob->getSymbolName());
    if (oNamedValueTypeKeys)
        valueTypeBlob->namedValueTypeKeys = *oNamedValueTypeKeys;

    return valueTypeBlob;
}

shared_ptr<ValueType> Analyzer::checkValueType(shared_ptr<ValueTypeBoxed> valueTypeBoxed) {
    // Skip if already resolved
    if (valueTypeBoxed->getBoxedValueType() != nullptr)
        return valueTypeBoxed;
    
    // Otherwise try getting value type from the provided scope
    if (!valueTypeBoxed->getNamedValueTypeKey()) {
        markErrorInvalidType(nullptr, valueTypeBoxed, nullptr);
        return nullptr;
    }
    
    // Try resolving named value type key (ignore failures, since it may be a blob field)
    shared_ptr<ValueType> valueType = scope->boxedScope->getNamedValueType(*valueTypeBoxed->getNamedValueTypeKey());
    shared_ptr<ValueTypeBoxed> clonedValueTypeBoxed = dynamic_pointer_cast<ValueTypeBoxed>(valueTypeBoxed->clone());
    if (valueType != nullptr) {
        clonedValueTypeBoxed->boxedValueType = valueType;
    }

    return clonedValueTypeBoxed;
}

shared_ptr<ValueType> Analyzer::checkValueType(shared_ptr<ValueTypeEnum> valueTypeEnum) {
    optional<vector<string>> oNamedValueTypeKeys = scope->enumScope->getNamedValueTypeKeys(valueTypeEnum->getSymbolName());

    // Check if enum is registered
    if (!oNamedValueTypeKeys) {
        markErrorNotDefined(nullptr, valueTypeEnum->getSymbolName()->getGlobalName());
        return nullptr;
    }

    // Check number of named types match
    if ((*oNamedValueTypeKeys).size() != valueTypeEnum->getNamedValueTypes().size()) {
        markErrorInvalidType(nullptr, valueTypeEnum, nullptr);
        return nullptr;
    }

    valueTypeEnum->namedValueTypeKeys = *oNamedValueTypeKeys;

    return valueTypeEnum;
}

shared_ptr<ValueType> Analyzer::checkValueType(shared_ptr<ValueTypeEnumField> valueTypeEnumField) {
    optional<vector<string>> oNamedValueTypeKeys = scope->enumScope->getNamedValueTypeKeys(valueTypeEnumField->getSymbolName());
    shared_ptr<ValueType> payloadValueType = scope->enumScope->getPayloadValueType(valueTypeEnumField->getSymbolName());

    // Check if enum field is registered
    if (!oNamedValueTypeKeys || payloadValueType == nullptr) {
        markErrorNotDefined(nullptr, valueTypeEnumField->getSymbolName()->getGlobalName());
        return nullptr;
    }

    // Check number of named types match
    if ((*oNamedValueTypeKeys).size() != valueTypeEnumField->getNamedValueTypes().size()) {
        markErrorInvalidType(nullptr, valueTypeEnumField, nullptr);
        return nullptr;
    }

    // Check payload type, first make sure that potential named types in boxed have access to the current context
    scope->pushLevel();
    scope->boxedScope->registerNamedValueTypesMap(*oNamedValueTypeKeys, valueTypeEnumField->getNamedValueTypes());
    payloadValueType = resolvedAndCheckedValueType(payloadValueType, false, nullptr);
    scope->popLevel();
    if (payloadValueType == nullptr) {
        markErrorInvalidType(nullptr, valueTypeEnumField, nullptr);
        return nullptr;
    }

    valueTypeEnumField->namedValueTypeKeys = *oNamedValueTypeKeys;
    valueTypeEnumField->payloadValueType = payloadValueType;

    return valueTypeEnumField;
}

shared_ptr<ValueType> Analyzer::checkValueType(shared_ptr<ValueTypeFun> valueTypeFun) {
    vector<shared_ptr<ValueType>> argValueTypes = valueTypeFun->getArgumentValueTypes();
    for (shared_ptr<ValueType> argValueType : argValueTypes) {
        if (resolvedAndCheckedValueType(argValueType, true, nullptr) == nullptr)
            return nullptr;
    }
    if (resolvedAndCheckedValueType(valueTypeFun->getReturnValueType(), true, nullptr) == nullptr)
        return nullptr;

    return valueTypeFun;
}

void Analyzer::markErrorAlreadyDefined(shared_ptr<Location> location, const string &identifier) {
    string message = format("\"{}\" is already defined", identifier);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidAttribute(shared_ptr<Location> location, const string &name) {
    string message = format("Invalid attribute {}", name);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidArgumentsCount(shared_ptr<Location> location, int actualCount, int expectedCount) {
    string message = format("Invalid arguments count {}, expected {}", actualCount, expectedCount);
    errors.push_back(Error::error(location, message));
}

 void Analyzer::markErrorInvalidBuiltIn(shared_ptr<Location> location, const string &builtInName, shared_ptr<ValueType> type) {
    string message = format("Invalid built-in \"{}\" on type {}", builtInName, Logger::toString(type));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidCast(shared_ptr<Location> location, shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    string message = format("Invalid cast from {} to {}", Logger::toString(sourceType), Logger::toString(targetType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidImport(shared_ptr<Location> location, const string &moduleName) {
    string message = format("Invalid import, module \"{}\" doesn't exist", moduleName);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidOperationBinary(shared_ptr<Location> location, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    string message = format("Invalid binary operation {} for types {} and {}", Logger::toString(operation), Logger::toString(firstType), Logger::toString(secondType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidOperationUnary(shared_ptr<Location> location, ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    string message = format("Invalid unary operation {} for type {}", Logger::toString(operation), Logger::toString(type));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidType(shared_ptr<Location> location, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType) {
    string message;
    if (expectedType != nullptr)
        message = format("Invalid type {}, expected {}", Logger::toString(actualType), Logger::toString(expectedType));
    else
        message = format(         "Invalid type {}", Logger::toString(actualType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorNotDefined(shared_ptr<Location> location, const string &name) {
    string message = format("{} is not defined in scope", name);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorNotImplemented(shared_ptr<Location> location, const string &protoName, const string &memberName) {
    string message = format("member `{}` of proto `{}` not implemented", memberName, protoName);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorUnexpectedExpression(shared_ptr<Location> location) {
    string message = format("Unexpected expression");
    errors.push_back(Error::error(location, message));
}

