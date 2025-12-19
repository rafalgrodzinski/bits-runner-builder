#include "Analyzer.h"

#include "Error.h"
#include "Logger.h"
#include "AnalyzerScope.h"
#include "Parser/ValueType.h"

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
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementFunctionDeclaration.h"
#include "Parser/Statement/StatementMetaExternFunction.h"
#include "Parser/Statement/StatementMetaExternVariable.h"
#include "Parser/Statement/StatementMetaImport.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementRawFunction.h"
#include "Parser/Statement/StatementRepeat.h"
#include "Parser/Statement/StatementReturn.h"
#include "Parser/Statement/StatementVariable.h"
#include "Parser/Statement/StatementVariableDeclaration.h"

Analyzer::Analyzer(
    vector<shared_ptr<Statement>> statements,
    vector<shared_ptr<Statement>> headerStatements,
    map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap
): statements(statements), headerStatements(headerStatements), importableHeaderStatementsMap(importableHeaderStatementsMap) { }

void Analyzer::checkModule() {
    scope = make_shared<AnalyzerScope>();

    // check header
    for (shared_ptr<Statement> statement : headerStatements)
        checkStatement(statement, nullptr);

    // check body
    for (shared_ptr<Statement> statement : statements)
        checkStatement(statement, nullptr);

    if (!errors.empty()) {
        for (shared_ptr<Error> &error : errors)
            Logger::print(error);
        exit(1);
    }
}

//
// Statements
//
void Analyzer::checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType) {
    switch (statement->getKind()) {
        case StatementKind::ASSIGNMENT:
            checkStatement(dynamic_pointer_cast<StatementAssignment>(statement));
            break;
        case StatementKind::BLOB:
            checkStatement(dynamic_pointer_cast<StatementBlob>(statement));
            break;
        case StatementKind::BLOB_DECLARATION:
            checkStatement(dynamic_pointer_cast<StatementBlobDeclaration>(statement));
            break;
        case StatementKind::BLOCK:
            checkStatement(dynamic_pointer_cast<StatementBlock>(statement), returnType);
            break;
        case StatementKind::EXPRESSION:
            checkStatement(dynamic_pointer_cast<StatementExpression>(statement), returnType);
            break;
        case StatementKind::FUNCTION:
            checkStatement(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        case StatementKind::FUNCTION_DECLARATION:
            checkStatement(dynamic_pointer_cast<StatementFunctionDeclaration>(statement));
            break;
        case StatementKind::META_EXTERN_FUNCTION:
            checkStatement(dynamic_pointer_cast<StatementMetaExternFunction>(statement));
            break;
        case StatementKind::META_EXTERN_VARIABLE:
            checkStatement(dynamic_pointer_cast<StatementMetaExternVariable>(statement));
            break;
        case StatementKind::META_IMPORT:
            checkStatement(dynamic_pointer_cast<StatementMetaImport>(statement));
            break;
        case StatementKind::MODULE:
            break;
        case StatementKind::REPEAT:
            checkStatement(dynamic_pointer_cast<StatementRepeat>(statement), returnType);
            break;
        case StatementKind::RAW_FUNCTION:
            checkStatement(dynamic_pointer_cast<StatementRawFunction>(statement));
            break;
        case StatementKind::RETURN:
            checkStatement(dynamic_pointer_cast<StatementReturn>(statement), returnType);
            break;
        case StatementKind::VARIABLE:
            checkStatement(dynamic_pointer_cast<StatementVariable>(statement));
            break;
        case StatementKind::VARIABLE_DECLARATION:
            checkStatement(dynamic_pointer_cast<StatementVariableDeclaration>(statement));
            break;
    }
}

void Analyzer::checkStatement(shared_ptr<StatementAssignment> statementAssignment) {
    shared_ptr<ValueType> targetType = typeForExpression(statementAssignment->getExpressionChained());
    if (targetType == nullptr)
        return;
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

void Analyzer::checkStatement(shared_ptr<StatementBlob> statementBlob) {
    vector<pair<string, shared_ptr<ValueType>>> members;
    for (auto &member : statementBlob->getMembers()) {
        if (member.second->isData()) {
            if (member.second->getCountExpression() == nullptr) {
                markErrorNotDefined(statementBlob->getLocation(), format("{}'s count expression", member.first));
                return;
            }
            member.second->getCountExpression()->valueType = typeForExpression(
                member.second->getCountExpression(),
                nullptr,
                nullptr
            );
        }
        members.push_back(pair(member.first, member.second));
    }

    shared_ptr<ValueType> valueType = ValueType::blob(statementBlob->getName());

    string name = importModulePrefix + statementBlob->getName();
    if (!scope->setBlobMembers(name, members, true))
        markErrorAlreadyDefined(statementBlob->getLocation(), statementBlob->getName());
}

void Analyzer::checkStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration) {
    string name = importModulePrefix + statementBlobDeclaration->getName();
    scope->setBlobMembers(name, {}, false);
}

void Analyzer::checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType) {
    for (shared_ptr<Statement> statement : statementBlock->getStatements())
        checkStatement(statement, returnType);
}

void Analyzer::checkStatement(shared_ptr<StatementExpression> statementExpression, shared_ptr<ValueType> returnType) {
    // returned value type is ignored
    statementExpression->getExpression()->valueType = typeForExpression(statementExpression->getExpression(), nullptr, returnType);
}

void Analyzer::checkStatement(shared_ptr<StatementFunction> statementFunction) {
    // updated types for count expressions
    for (auto &argument : statementFunction->getArguments()) {
        if (shared_ptr<Expression> countExpression = argument.second->getCountExpression())
            countExpression->valueType = typeForExpression(countExpression, nullptr, nullptr);
    }

    // update return's type for count expression
    if (statementFunction->getReturnValueType()->isData()) {
        // returned data type should have specified size
        if (statementFunction->getReturnValueType()->getCountExpression() == nullptr) {
            markErrorInvalidType(statementFunction->getLocation(), statementFunction->getReturnValueType(), nullptr);
            return;
        }
        statementFunction->getReturnValueType()->getCountExpression()->valueType = typeForExpression(
            statementFunction->getReturnValueType()->getCountExpression(),
            nullptr,
            nullptr
        );
    }

    // check if function is not yet defined and register it
    if (!scope->setFunctionType(statementFunction->getName(), statementFunction->getValueType(), true))
        markErrorAlreadyDefined(statementFunction->getLocation(), statementFunction->getName());

    scope->pushLevel();
    // register arguments as variables
    for (auto &argument : statementFunction->getArguments())
        scope->setVariableType(argument.first, argument.second, true);

    checkStatement(statementFunction->getStatementBlock(), statementFunction->getReturnValueType());
    scope->popLevel();
}

void Analyzer::checkStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration) {
    // updated types for count expressions
    for (auto &argument : statementFunctionDeclaration->getArguments()) {
        if (shared_ptr<Expression> countExpression = argument.second->getCountExpression())
            countExpression->valueType = typeForExpression(countExpression, nullptr, nullptr);
    }

    // update return's type for count expression
    if (statementFunctionDeclaration->getReturnValueType()->isData()) {
        // returned data type should have specified size
        if (statementFunctionDeclaration->getReturnValueType()->getCountExpression() == nullptr) {
            markErrorInvalidType(statementFunctionDeclaration->getLocation(), statementFunctionDeclaration->getReturnValueType(), nullptr);
            return;
        }
        statementFunctionDeclaration->getReturnValueType()->getCountExpression()->valueType = typeForExpression(
            statementFunctionDeclaration->getReturnValueType()->getCountExpression(),
            nullptr,
            nullptr
        );
    }

    string name = importModulePrefix + statementFunctionDeclaration->getName();

    if (!scope->setFunctionType(name, statementFunctionDeclaration->getValueType(), false)) {
        markErrorInvalidType(
            statementFunctionDeclaration->getLocation(),
            statementFunctionDeclaration->getValueType(),
            nullptr
        );
    }
}

void Analyzer::checkStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction) {
    // updated types for count expressions
    for (auto &argument : statementMetaExternFunction->getArguments()) {
        if (shared_ptr<Expression> countExpression = argument.second->getCountExpression())
            countExpression->valueType = typeForExpression(countExpression, nullptr, nullptr);
    }

    // update return's type for count expression
    if (statementMetaExternFunction->getReturnValueType()->isData()) {
        statementMetaExternFunction->getReturnValueType()->getCountExpression()->valueType = typeForExpression(
            statementMetaExternFunction->getReturnValueType()->getCountExpression(),
            nullptr,
            nullptr
        );
    }

    if (!scope->setFunctionType(statementMetaExternFunction->getName(), statementMetaExternFunction->getValueType(), false))
        markErrorAlreadyDefined(statementMetaExternFunction->getLocation(), statementMetaExternFunction->getName());
}

void Analyzer::checkStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable) {
    string identifier = importModulePrefix + statementMetaExternVariable->getIdentifier();

    if (!scope->setVariableType(identifier, statementMetaExternVariable->getValueType(), false))
        markErrorAlreadyDefined(statementMetaExternVariable->getLocation(), identifier);
}

void Analyzer::checkStatement(shared_ptr<StatementMetaImport> statementMetaImport) {
    auto it = importableHeaderStatementsMap.find(statementMetaImport->getName());
    if (it == importableHeaderStatementsMap.end()) {
        markErrorInvalidImport(statementMetaImport->getLocation(), statementMetaImport->getName());
        return;
    }
    importModulePrefix = statementMetaImport->getName() + ".";
    for (shared_ptr<Statement> &importStatement : it->second) {
        checkStatement(importStatement, nullptr);
    }
    importModulePrefix = "";
}

void Analyzer::checkStatement(shared_ptr<StatementRawFunction> statementRawFunction) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementRawFunction->getArguments())
        argumentTypes.push_back(argument.second);

    if (!scope->setFunctionType(statementRawFunction->getName(), statementRawFunction->getValueType(), true))
        markErrorAlreadyDefined(statementRawFunction->getLocation(), statementRawFunction->getName());
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

    if (expressionType == nullptr || !expressionType->isEqual(returnType))
        markErrorInvalidType(
            statementReturn->getLocation(),
            expressionType,
            returnType
        );
}

void Analyzer::checkStatement(shared_ptr<StatementVariable> statementVariable) {
    // Update count expression
    if (statementVariable->getValueType()->getCountExpression() != nullptr)
        statementVariable->getValueType()->getCountExpression()->valueType = typeForExpression(statementVariable->getValueType()->getCountExpression(), nullptr, nullptr);

    // check if specified blob type is valid
    if (statementVariable->getValueType()->isBlob()) {
        if(!scope->getBlobMembers(*(statementVariable->getValueType()->getBlobName()))) {
            markErrorInvalidType(statementVariable->getLocation(), statementVariable->getValueType(), nullptr);
        }
    }

    if (statementVariable->getExpression() != nullptr) {
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

        if (!statementVariable->getValueType()->isEqual(statementVariable->getExpression()->getValueType()))
            markErrorInvalidType(statementVariable->getExpression()->getLocation(), statementVariable->getExpression()->getValueType(), statementVariable->getValueType());
    }

    if (!scope->setVariableType(statementVariable->getIdentifier(), statementVariable->getValueType(), true))
        markErrorAlreadyDefined(statementVariable->getLocation(), statementVariable->getIdentifier());

    // updated corresponding variable declaration
    for (shared_ptr<Statement> headerStatement : this->headerStatements) {
        // find matching declaration
        shared_ptr<StatementVariableDeclaration> statementVariableDeclaration = dynamic_pointer_cast<StatementVariableDeclaration>(headerStatement);
        if (statementVariableDeclaration != nullptr && statementVariableDeclaration->getIdentifier().compare(statementVariable->getIdentifier()) == 0) {
            statementVariableDeclaration->valueType = statementVariable->getValueType();
        }
    }
}

void Analyzer::checkStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration) {
    string identifier = importModulePrefix + statementVariableDeclaration->getIdentifier();

    if (!scope->setVariableType(identifier, statementVariableDeclaration->getValueType(), false))
        markErrorAlreadyDefined(statementVariableDeclaration->getLocation(), identifier);
}

//
// Expressions
//
shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<Expression> expression, shared_ptr<Expression> parentExpression, shared_ptr<ValueType> returnType) {
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
    // try auto cross-casting
    shared_ptr<ValueType> targetType;

    targetType = typeForExpression(expressionBinary->getRight(), nullptr, nullptr);
    if (targetType == nullptr) {
        markErrorInvalidType(expressionBinary->getRight()->getLocation(), targetType, nullptr);
        return nullptr;
    }
    expressionBinary->left = checkAndTryCasting(
        expressionBinary->getLeft(),
        targetType,
        nullptr
    );
    if (expressionBinary->getLeft() == nullptr)
        return nullptr;

    if (expressionBinary->getLeft()->getValueType() == nullptr)
        return nullptr;

    targetType = typeForExpression(expressionBinary->getLeft(), nullptr, nullptr);
    if (targetType == nullptr) {
        markErrorInvalidType(expressionBinary->getLeft()->getLocation(), targetType, nullptr);
        return nullptr;
    }
    expressionBinary->right = checkAndTryCasting(
        expressionBinary->getRight(),
        targetType,
        nullptr
    );
    if (expressionBinary->getRight()->getValueType() == nullptr)
        return nullptr;

    // validate types
    ExpressionBinaryOperation operation = expressionBinary->getOperation();
    shared_ptr<ValueType> firstType = expressionBinary->getLeft()->getValueType();
    shared_ptr<ValueType> secondType = expressionBinary->getRight()->getValueType();

    if (firstType == nullptr || secondType == nullptr)
        return nullptr;

    if (!isBinaryOperationValidForTypes(operation, firstType, secondType)) {
        markErrorInvalidOperationBinary(expressionBinary->getLocation(), operation, firstType, secondType);
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

    // check for built-in
    if (parentExpression != nullptr) {
        bool isPointer = parentExpression->getValueType()->isPointer();
        bool isVal = expressionCall->getName().compare("val") == 0;

        if (isPointer && isVal && parentExpression->getValueType()->getSubType()->isFunction()) {
            valueType = parentExpression->getValueType()->getSubType();
        } else {
            markErrorInvalidType(expressionCall->getLocation(), parentExpression->getValueType()->getSubType(), nullptr);
            return nullptr;
        }
    } else {
        valueType = scope->getFunctionType(expressionCall->getName());

        // check if defined
        if (valueType == nullptr) {
            markErrorNotDefined(expressionCall->getLocation(), expressionCall->getName());
            return nullptr;
        }
    }

    // check arguments count
    vector<shared_ptr<ValueType>> argumentTypes = *(valueType->getArgumentTypes());
    if (argumentTypes.size() != expressionCall->getArgumentExpressions().size()) {
        markErrorInvalidArgumentsCount(
            expressionCall->getLocation(),
            expressionCall->getArgumentExpressions().size(),
            argumentTypes.size()
        );
    // check argument types
    } else {
        for (int i=0; i<argumentTypes.size(); i++) {
            shared_ptr<ValueType> targetType = argumentTypes.at(i);

            expressionCall->argumentExpressions[i] = checkAndTryCasting(
                expressionCall->getArgumentExpressions().at(i),
                targetType,
                valueType->getReturnType()
            );
            if (expressionCall->getArgumentExpressions().at(i) == nullptr)
                return nullptr;

            shared_ptr<ValueType> sourceType = expressionCall->getArgumentExpressions().at(i)->getValueType();
            if (sourceType == nullptr)
                return nullptr;

            if (!sourceType->isEqual(targetType)) {
                markErrorInvalidType(
                    expressionCall->getArgumentExpressions().at(i)->getLocation(),
                    sourceType,
                    targetType
                );
                expressionCall->valueType = nullptr;
                return nullptr;
            }
        }
    }

    expressionCall->valueType = valueType->getReturnType();
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

    // if the first expression in a chain is a cast, we may want to do a built-in operation on it
    if (parentExpression == nullptr)
        return expressionCast->getValueType();

    bool isSourceNumeric = parentExpression->getValueType()->isNumeric();

    bool areNumeric = parentExpression->getValueType()->isNumeric() && expressionCast->getValueType()->isNumeric();
    bool areBool = parentExpression->getValueType()->isBool() && expressionCast->getValueType()->isBool();
    bool areDataNumeric = parentExpression->getValueType()->isDataNumeric() && expressionCast->getValueType()->isDataNumeric();
    bool areDataBool = parentExpression->getValueType()->isDataBool() && expressionCast->getValueType()->isDataBool();

    if (areNumeric || areBool || areDataNumeric || areDataBool) {
        // if cast has not count expression, use one from the parent expression
        if (expressionCast->getValueType()->isData() && expressionCast->getValueType()->getCountExpression() == nullptr) {
            expressionCast->valueType = ValueType::data(
                expressionCast->getValueType()->getSubType(),
                parentExpression->getValueType()->getCountExpression()
            );
        }
        return expressionCast->getValueType();
    } else {
        markErrorInvalidCast(expressionCast->getLocation(), parentExpression->getValueType(), expressionCast->getValueType());
        return nullptr;
    }
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionChained> expressionChained) {
    shared_ptr<Expression> parentExpression = nullptr;

    for (shared_ptr<Expression> chainExpression : expressionChained->getChainExpressions()) {
        shared_ptr<ValueType> chainType = typeForExpression(chainExpression, parentExpression, nullptr);
        chainExpression->valueType = chainType;
        parentExpression = chainExpression;
        if (chainType == nullptr)
            return nullptr;
    }

    expressionChained->valueType = parentExpression->getValueType();
    return expressionChained->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral) {
    // if the type is already figured out, we should skip the detection (it may already be promoted to data, blob, or ptr)
    if (expressionCompositeLiteral->getValueType() != nullptr)
        return expressionCompositeLiteral->getValueType();

    vector<shared_ptr<ValueType>> elementTypes;
    for (shared_ptr<Expression> expression : expressionCompositeLiteral->getExpressions()) {
        if (expression == nullptr)
            return nullptr;
        shared_ptr<ValueType> elementType = typeForExpression(expression, nullptr, nullptr);
        if (elementType == nullptr)
            return nullptr;
        elementTypes.push_back(elementType);
    }
    shared_ptr<Expression> countExpression = ExpressionLiteral::expressionLiteralForInt(elementTypes.size(), expressionCompositeLiteral->getLocation());
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
        case ExpressionLiteralKind::INT:
            expressionLiteral->valueType =  ValueType::INT;
            break;
        case ExpressionLiteralKind::FLOAT:
            expressionLiteral->valueType =  ValueType::FLOAT;
            break;
        default:
            markErrorInvalidType(expressionLiteral->getLocation(), nullptr, nullptr);
    }

    return expressionLiteral->getValueType();
}

shared_ptr<ValueType> Analyzer::typeForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    ExpressionUnaryOperation operation = expressionUnary->getOperation();
    shared_ptr<ValueType> subType = typeForExpression(expressionUnary->getSubExpression(), nullptr, nullptr);

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
        bool isData = parentExpression->getValueType()->isData();
        bool isPointer = parentExpression->getValueType()->isPointer();
        bool isBlob = parentExpression->getValueType()->isBlob();

        bool isCount = expressionValue->getIdentifier().compare("count") == 0;
        bool isVal = expressionValue->getIdentifier().compare("val") == 0;
        bool isVadr = expressionValue->getIdentifier().compare("vAdr") == 0;
        bool isAdr = expressionValue->getIdentifier().compare("adr") == 0;
        bool isSize = expressionValue->getIdentifier().compare("size") == 0;


        if (isData && isCount) {
            expressionValue->valueType = ValueType::INT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_COUNT;
            return expressionValue->getValueType();
        } else if (isPointer && isVal) {
            switch (expressionValue->getValueKind()) {
                case ExpressionValueKind::SIMPLE:
                case ExpressionValueKind::BUILT_IN_VAL_SIMPLE:
                    expressionValue->valueType = parentExpression->getValueType()->getSubType();
                    expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_SIMPLE;
                    break;
                case ExpressionValueKind::DATA:
                case ExpressionValueKind::BUILT_IN_VAL_DATA:
                    expressionValue->valueType = parentExpression->getValueType()->getSubType()->getSubType();
                    expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VAL_DATA;
                    expressionValue->getIndexExpression()->valueType = typeForExpression(expressionValue->getIndexExpression(), nullptr, nullptr);
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
        } else if (isPointer && isVadr) {
            expressionValue->valueType = ValueType::INT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_VADR;
            return expressionValue->getValueType();
        } else if (isAdr) {
            expressionValue->valueType = ValueType::INT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_ADR;
            return expressionValue->getValueType();
        } else if (isSize) {
            expressionValue->valueType = ValueType::INT;
            expressionValue->valueKind = ExpressionValueKind::BUILT_IN_SIZE;
            return expressionValue->getValueType();
        // Invalid built-in call
        } else if (isCount || isVal || isVadr) {
            markErrorInvalidBuiltIn(expressionValue->getLocation(), expressionValue->getIdentifier(), parentExpression->getValueType());
            expressionValue->valueType = nullptr;
            return expressionValue->getValueType();
        // check blob member
        } else if (isBlob) {
            string blobName = *(parentExpression->getValueType()->getBlobName());
            optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = scope->getBlobMembers(blobName);
            if (blobMembers) {
                for (pair<string, shared_ptr<ValueType>> &blobMember : *blobMembers) {
                    if (expressionValue->getIdentifier().compare(blobMember.first) == 0) {
                        // found corresponding blob, decide if it's a simple or data access
                        switch (expressionValue->getValueKind()) {
                            case ExpressionValueKind::SIMPLE:
                                expressionValue->valueType = blobMember.second;
                                return expressionValue->getValueType();
                            case ExpressionValueKind::DATA:
                                expressionValue->valueType = blobMember.second->getSubType();
                                expressionValue->getIndexExpression()->valueType = typeForExpression(expressionValue->getIndexExpression(), nullptr, nullptr);
                                return expressionValue->getValueType();
                            default:
                                break;
                        }
                    }
                }
            }
            markErrorNotDefined(
                expressionValue->getLocation(),
                format("{}.{}", blobName, expressionValue->getIdentifier())
            );
            return nullptr;
        }
    }

    // first assume just simple
    shared_ptr<ValueType> type = scope->getVariableType(expressionValue->getIdentifier());
    if (type != nullptr)
        expressionValue->valueKind = ExpressionValueKind::SIMPLE;

    // then check if data
    if (type != nullptr && expressionValue->getIndexExpression() != nullptr) {
        shared_ptr<Expression> indexExpression = expressionValue->getIndexExpression();
        indexExpression->valueType = typeForExpression(indexExpression, nullptr, nullptr);
        if (indexExpression->getValueType() == nullptr)
            return nullptr;
        if (!indexExpression->getValueType()->isInteger())
            markErrorInvalidType(indexExpression->getLocation(), indexExpression->getValueType(), ValueType::INT);
        type = type->getSubType();
        expressionValue->valueKind = ExpressionValueKind::DATA;
    // finally check if it's a function
    } else if (type == nullptr) {
        if (type = scope->getFunctionType(expressionValue->getIdentifier()))
            expressionValue->valueKind = ExpressionValueKind::FUN;
    }

    if (type == nullptr)
        markErrorNotDefined(expressionValue->getLocation(), expressionValue->getIdentifier());

    expressionValue->valueType = type;
    return expressionValue->getValueType();
}

//
// Support
//
bool Analyzer::isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    switch (type->getKind()) {
        case ValueTypeKind::BOOL:
            switch (operation) {
                case ExpressionUnaryOperation::NOT: {
                    return true;
                }
                default:
                    break;
            break;
        }
        case ValueTypeKind::INT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:
        case ValueTypeKind::S8:
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
        default:
            break;
    }

    // all other combinations are invalid
    return false;
}

bool Analyzer::isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    bool areTypesMatcing = firstType->isEqual(secondType);

    switch (firstType->getKind()) {
        // Valid operations for boolean types
        case ValueTypeKind::BOOL: {
            switch (operation) {
                case ExpressionBinaryOperation::EQUAL:
                case ExpressionBinaryOperation::NOT_EQUAL:

                case ExpressionBinaryOperation::OR:
                case ExpressionBinaryOperation::XOR:
                case ExpressionBinaryOperation::AND:
                    return areTypesMatcing;
                default:
                    break;
            }
            break;
        }
        // Valid operations for numeric types
        case ValueTypeKind::INT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:
        case ValueTypeKind::S8:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64: {
            switch (operation) {
                // shift operations requires second type to be numeric
                case ExpressionBinaryOperation::BIT_SHL:
                case ExpressionBinaryOperation::BIT_SHR: {
                    switch (secondType->getKind()) {
                        case ValueTypeKind::INT:
                        case ValueTypeKind::U8:
                        case ValueTypeKind::U32:
                        case ValueTypeKind::U64:
                        case ValueTypeKind::S8:
                        case ValueTypeKind::S32:
                        case ValueTypeKind::S64: {
                            return true;
                        }
                        default:
                            break;
                    }
                    break;
                }

                // other operations have to match
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
                    return areTypesMatcing;
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

shared_ptr<ValueType> Analyzer::typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    switch (operation) {
        case ExpressionUnaryOperation::MINUS:
            switch (type->getKind()) {
                case ValueTypeKind::INT:
                    return ValueType::INT;
                case ValueTypeKind::U8:
                    return ValueType::S8;
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

 shared_ptr<ValueType> Analyzer::typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    switch (operation) {
        case ExpressionBinaryOperation::EQUAL:
        case ExpressionBinaryOperation::NOT_EQUAL:
        case ExpressionBinaryOperation::LESS:
        case ExpressionBinaryOperation::LESS_EQUAL:
        case ExpressionBinaryOperation::GREATER:
        case ExpressionBinaryOperation::GREATER_EQUAL:
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

    if (!canCast(sourceType, targetType))
        return sourceExpression;

    // single literal just needs to set the type
    if (sourceExpression->getKind() == ExpressionKind::LITERAL) {
        sourceExpression->valueType = targetType;
        return sourceExpression;
    // composite to blob
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isBlob()) {
        sourceExpression->valueType = targetType;
        vector<pair<string, shared_ptr<ValueType>>> blobMembers = *(scope->getBlobMembers(*(targetType->getBlobName())));
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        for (int i=0; i<blobMembers.size(); i++) {
            shared_ptr<Expression> sourceMemberExpression = expressionCompositeLiteral->getExpressions().at(i);
            sourceMemberExpression = checkAndTryCasting(sourceMemberExpression, blobMembers.at(i).second, returnType);
        }
        return sourceExpression;
    // composite to data
    } else if (sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL && targetType->isData()) {
        shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral = dynamic_pointer_cast<ExpressionCompositeLiteral>(sourceExpression);
        // first update the type
        sourceExpression->valueType = ValueType::data(
            targetType->getSubType(),
            ExpressionLiteral::expressionLiteralForInt(
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
        return sourceExpression;
    // data to data
    } else if (sourceExpression->getValueType()->isData() && targetType->isData()) {
        if (sourceType->getCountExpression() != nullptr)
            sourceType->getCountExpression()->valueType = typeForExpression(sourceType->getCountExpression(), nullptr, returnType);

        if (targetType->getCountExpression() == nullptr)
            return sourceExpression;
    }

    // if target has no count expression defined, use the one from source
    if (targetType->isData() && targetType->getCountExpression() == nullptr) {
        targetType = ValueType::data(
            targetType->getSubType(),
            sourceExpression->getValueType()->getCountExpression()
        );
    }

    shared_ptr<ExpressionChained> targetExpression = make_shared<ExpressionChained>(
        vector<shared_ptr<Expression>>(
            {
                sourceExpression,
                make_shared<ExpressionCast>(targetType, sourceExpression->getLocation())
            }
        ),
        sourceExpression->getLocation()
    );

    targetExpression->valueType = targetType;
    return targetExpression;
}

bool Analyzer::canCast(shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    switch (sourceType->getKind()) {
        // unsigned
        case ValueTypeKind::INT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::U8:
                case ValueTypeKind::U32:
                case ValueTypeKind::U64:

                case ValueTypeKind::S8:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                default:
                    return false;
            }
            break;
        }
        // signed
        case ValueTypeKind::S8:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::S8:
                case ValueTypeKind::S32:
                case ValueTypeKind::S64:

                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                default:
                    return false;
            }
            break;
        }
        // float
        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64: {
            switch (targetType->getKind()) {
                case ValueTypeKind::F32:
                case ValueTypeKind::F64:
                    return true;

                default:
                    return false;
            }
            break;
        }
        // data
        case ValueTypeKind::DATA: {
            switch (targetType->getKind()) {
                case ValueTypeKind::DATA:
                    return canCast(sourceType->getSubType(), targetType->getSubType());

                default:
                    return false;
            }
            break;
        }
        // composite
        case ValueTypeKind::COMPOSITE: {
            switch (targetType->getKind()) {
                case ValueTypeKind::BLOB: {
                    optional<string> blobName = targetType->getBlobName();
                    if (!targetType->getBlobName())
                        return false;
                    optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = scope->getBlobMembers(*blobName);
                    if (!blobMembers)
                        return false;
                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());

                    // check that number of memebrs match
                    if (sourceElementTypes.size() !=  (*blobMembers).size())
                        return false;
                    
                    // check that each entry in composite can be cast to member in blob
                    for (int i=0; i<((*blobMembers).size()); i++) {
                        if (!canCast(sourceElementTypes.at(i), (*blobMembers).at(i).second))
                            return false;
                    }
                    return true;
                }
                case ValueTypeKind::DATA: {
                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());
                    for (shared_ptr<ValueType> sourceElementType : sourceElementTypes) {
                        if (!canCast(sourceElementType, targetType->getSubType()))
                            return false;
                    }
                    return true;
                }
                case ValueTypeKind::PTR: {
                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());
                    return sourceElementTypes.size() == 1 && (sourceElementTypes.at(0)->getKind() == ValueTypeKind::INT || sourceElementTypes.at(0)->isUnsignedInteger());
                }

                default:
                    return false;
            }
            break;
        }
        // blob
        case ValueTypeKind::BLOB: {
            if (!targetType->isBlob())
                return false;

            string sourceBlobName = *(sourceType->getBlobName());
            string targetBlobName = *(targetType->getBlobName());

            return sourceBlobName.compare(targetBlobName) == 0;
        }

        default:
            return false;
    }
}

void Analyzer::markErrorAlreadyDefined(shared_ptr<Location> location, string identifier) {
    string message = format("\"{}\" is already defined", identifier);
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidArgumentsCount(shared_ptr<Location> location, int actualCount, int expectedCount) {
    string message = format("Invalid arguments count {}, expected {}", actualCount, expectedCount);
    errors.push_back(Error::error(location, message));
}

 void Analyzer::markErrorInvalidBuiltIn(shared_ptr<Location> location, string builtInName, shared_ptr<ValueType> type) {
    string message = format("Invalid built-in \"{}\" on type {}", builtInName, Logger::toString(type));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidCast(shared_ptr<Location> location, shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    string message = format("Invalid cast from {} to {}", Logger::toString(sourceType), Logger::toString(targetType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorInvalidImport(shared_ptr<Location> location, string moduleName) {
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
        message = format("Invalid type {}", Logger::toString(actualType));
    errors.push_back(Error::error(location, message));
}

void Analyzer::markErrorNotDefined(shared_ptr<Location> location, string identifier) {
    string message = format("\"{}\" not defined", identifier);
    errors.push_back(Error::error(location, message));
}

