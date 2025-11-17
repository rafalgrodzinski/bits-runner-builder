#include "TypesAnalyzer.h"

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

TypesAnalyzer::TypesAnalyzer(
    vector<shared_ptr<Statement>> statements,
    vector<shared_ptr<Statement>> headerStatements,
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap
): statements(statements), headerStatements(headerStatements), exportedHeaderStatementsMap(exportedHeaderStatementsMap) { }

void TypesAnalyzer::checkModule() {
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
void TypesAnalyzer::checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType) {
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

void TypesAnalyzer::checkStatement(shared_ptr<StatementAssignment> statementAssignment) {
    shared_ptr<ValueType> targetType = typeForExpression(statementAssignment->getExpressionChained());
    statementAssignment->valueExpression = checkAndTryCasting(statementAssignment->getValueExpression(), targetType, nullptr);

    shared_ptr<ValueType> sourceType = statementAssignment->getValueExpression()->getValueType();
    if (sourceType != nullptr && !sourceType->isEqual(targetType)) {
        markErrorInvalidType(
            statementAssignment->getExpressionChained()->getLine(),
            statementAssignment->getExpressionChained()->getColumn(),
            sourceType,
            targetType
        );
    }
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementBlob> statementBlob) {
    vector<pair<string, shared_ptr<ValueType>>> members;
    for (auto &member : statementBlob->getMembers())
        members.push_back(pair(member.first, member.second));

    shared_ptr<ValueType> valueType = ValueType::blob(statementBlob->getName());

    if (!scope->setBlobMembers(statementBlob->getName(), members, true))
        markErrorAlreadyDefined(statementBlob->getLine(), statementBlob->getColumn(), statementBlob->getName());
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration) {
    scope->setBlobMembers(statementBlobDeclaration->getName(), {}, false);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType) {
    for (shared_ptr<Statement> statement : statementBlock->getStatements())
        checkStatement(statement, returnType);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementExpression> statementExpression, shared_ptr<ValueType> returnType) {
    // returned value type is ignored
    statementExpression->getExpression()->valueType = typeForExpression(statementExpression->getExpression(), nullptr, returnType);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementFunction> statementFunction) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementFunction->getArguments())
        argumentTypes.push_back(argument.second);

    if (!scope->setFunctionType(statementFunction->getName(), statementFunction->getValueType(), true))
        markErrorAlreadyDefined(statementFunction->getLine(), statementFunction->getColumn(), statementFunction->getName());

    scope->pushLevel();
    // register arguments as variables
    for (auto &argument : statementFunction->getArguments())
        scope->setVariableType(argument.first, argument.second, true);

    checkStatement(statementFunction->getStatementBlock(), statementFunction->getReturnValueType());
    scope->popLevel();
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementFunctionDeclaration->getArguments())
        argumentTypes.push_back(argument.second);

    string name = importModulePrefix + statementFunctionDeclaration->getName();

    if (!scope->setFunctionType(name, statementFunctionDeclaration->getValueType(), false)) {
        markErrorInvalidType(
            statementFunctionDeclaration->getLine(),
            statementFunctionDeclaration->getColumn(),
            statementFunctionDeclaration->getValueType(),
            nullptr
        );
    }
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementMetaExternFunction->getArguments())
        argumentTypes.push_back(argument.second);

    if (!scope->setFunctionType(statementMetaExternFunction->getName(), statementMetaExternFunction->getValueType(), false))
        markErrorAlreadyDefined(statementMetaExternFunction->getLine(), statementMetaExternFunction->getColumn(), statementMetaExternFunction->getName());
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable) {
    string identifier = importModulePrefix + statementMetaExternVariable->getIdentifier();

    if (!scope->setVariableType(identifier, statementMetaExternVariable->getValueType(), false))
        markErrorAlreadyDefined(statementMetaExternVariable->getLine(), statementMetaExternVariable->getColumn(), identifier);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementMetaImport> statementMetaImport) {
    auto it = exportedHeaderStatementsMap.find(statementMetaImport->getName());
    if (it == exportedHeaderStatementsMap.end()) {
        markErrorInvalidImport(statementMetaImport->getLine(), statementMetaImport->getColumn(), statementMetaImport->getName());
        return;
    }
    importModulePrefix = statementMetaImport->getName() + ".";
    for (shared_ptr<Statement> &importStatement : it->second) {
        checkStatement(importStatement, nullptr);
    }
    importModulePrefix = "";
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementRawFunction> statementRawFunction) {
    // store arguments and return type
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : statementRawFunction->getArguments())
        argumentTypes.push_back(argument.second);

    if (!scope->setFunctionType(statementRawFunction->getName(), statementRawFunction->getValueType(), true))
        markErrorAlreadyDefined(statementRawFunction->getLine(), statementRawFunction->getColumn(), statementRawFunction->getName());
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementRepeat> statementRepeat, shared_ptr<ValueType> returnType) {
    scope->pushLevel();
    if (statementRepeat->getInitStatement() != nullptr)
        checkStatement(statementRepeat->getInitStatement(), returnType);

    if (statementRepeat->getPostStatement() != nullptr)
        checkStatement(statementRepeat->getPostStatement(), returnType);

    shared_ptr<Expression> preConditionExpression = statementRepeat->getPreConditionExpression();
    if (preConditionExpression != nullptr) {
        preConditionExpression->valueType = typeForExpression(preConditionExpression, nullptr, nullptr);
        if (preConditionExpression->getValueType() != nullptr && !preConditionExpression->getValueType()->isEqual(ValueType::BOOL))
            markErrorInvalidType(preConditionExpression->getLine(), preConditionExpression->getColumn(), preConditionExpression->getValueType(), ValueType::BOOL);
    }

    shared_ptr<Expression> postConditionExpression = statementRepeat->getPostConditionExpression();
    if (postConditionExpression != nullptr) {
        postConditionExpression->valueType = typeForExpression(postConditionExpression, nullptr, nullptr);
        if (postConditionExpression->getValueType() != nullptr && !postConditionExpression->getValueType()->isEqual(ValueType::BOOL))
            markErrorInvalidType(postConditionExpression->getLine(), postConditionExpression->getColumn(), postConditionExpression->getValueType(), ValueType::BOOL);
    }

    // body
    checkStatement(statementRepeat->getBodyBlockStatement(), returnType);
    scope->popLevel();
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType) {
    statementReturn->expression = checkAndTryCasting(
        statementReturn->getExpression(),
        returnType,
        returnType
    );

    shared_ptr<ValueType> expressionType = statementReturn->getExpression()->getValueType();

    if (expressionType == nullptr || !expressionType->isEqual(returnType))
        markErrorInvalidType(
            statementReturn->getLine(),
            statementReturn->getColumn(),
            expressionType,
            returnType
        );
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementVariable> statementVariable) {
    if (statementVariable->getExpression() != nullptr) {
        statementVariable->expression = checkAndTryCasting(
            statementVariable->getExpression(),
            statementVariable->getValueType(),
            nullptr
        );

        if (!statementVariable->getValueType()->isEqual(statementVariable->getExpression()->getValueType()))
            markErrorInvalidType(statementVariable->getExpression()->getLine(), statementVariable->getExpression()->getColumn(), statementVariable->getExpression()->getValueType(), statementVariable->getValueType());
    }

    if (!scope->setVariableType(statementVariable->getIdentifier(), statementVariable->getValueType(), true))
        markErrorAlreadyDefined(statementVariable->getLine(), statementVariable->getColumn(), statementVariable->getIdentifier());
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration) {
    string identifier = importModulePrefix + statementVariableDeclaration->getIdentifier();

    if (!scope->setVariableType(identifier, statementVariableDeclaration->getValueType(), false))
        markErrorAlreadyDefined(statementVariableDeclaration->getLine(), statementVariableDeclaration->getColumn(), identifier);
}

//
// Expressions
//
shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<Expression> expression, shared_ptr<Expression> parentExpression, shared_ptr<ValueType> returnType) {
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

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionBinary> expressionBinary) {
    // try auto cross-casting
    expressionBinary->left = checkAndTryCasting(
        expressionBinary->getLeft(),
        typeForExpression(expressionBinary->getRight(), nullptr, nullptr),
        nullptr
    );
    expressionBinary->right = checkAndTryCasting(
        expressionBinary->getRight(),
        typeForExpression(expressionBinary->getLeft(), nullptr, nullptr),
        nullptr
    );

    // validate types
    ExpressionBinaryOperation operation = expressionBinary->getOperation();
    shared_ptr<ValueType> firstType = expressionBinary->getLeft()->getValueType();
    shared_ptr<ValueType> secondType = expressionBinary->getRight()->getValueType();

    if (firstType == nullptr || secondType == nullptr)
        return nullptr;

    if (!isBinaryOperationValidForTypes(operation, firstType, secondType)) {
        markErrorInvalidOperationBinary(expressionBinary->getLine(), expressionBinary->getColumn(), operation, firstType, secondType);
        return nullptr;
    }

    expressionBinary->valueType = typeForBinaryOperation(operation, firstType, secondType);
    return expressionBinary->getValueType();
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionBlock> expressionBlock, shared_ptr<ValueType> returnType) {
    checkStatement(expressionBlock->getStatementBlock(), returnType);
    checkStatement(expressionBlock->getResultStatementExpression(), returnType);
    expressionBlock->valueType = expressionBlock->getResultStatementExpression()->getExpression()->getValueType();
    return expressionBlock->getValueType();
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionCall> expressionCall, shared_ptr<Expression> parentExpression) {
    shared_ptr<ValueType> valueType;

    // check for built-in
    if (parentExpression != nullptr) {
        bool isPointer = parentExpression->getValueType()->isPointer();
        bool isVal = expressionCall->getName().compare("val") == 0;

        if (isPointer && isVal && parentExpression->getValueType()->getSubType()->isFunction()) {
            valueType = parentExpression->getValueType()->getSubType();
        } else {
            markErrorInvalidType(expressionCall->getLine(), expressionCall->getColumn(), parentExpression->getValueType()->getSubType(), nullptr);
            return nullptr;
        }
    } else {
        valueType = scope->getFunctionType(expressionCall->getName());

        // check if defined
        if (valueType == nullptr) {
            markErrorNotDefined(expressionCall->getLine(), expressionCall->getColumn(), expressionCall->getName());
            return nullptr;
        }
    }

    // check arguments count
    vector<shared_ptr<ValueType>> argumentTypes = *(valueType->getArgumentTypes());
    if (argumentTypes.size() != expressionCall->getArgumentExpressions().size()) {
        markErrorInvalidArgumentsCount(
            expressionCall->getLine(),
            expressionCall->getColumn(),
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

            shared_ptr<ValueType> sourceType = expressionCall->getArgumentExpressions().at(i)->getValueType();
            if (sourceType == nullptr)
                return nullptr;

            if (!sourceType->isEqual(targetType)) {
                markErrorInvalidType(
                    expressionCall->getArgumentExpressions().at(i)->getLine(),
                    expressionCall->getArgumentExpressions().at(i)->getColumn(),
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

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionCast> expressionCast, shared_ptr<Expression> parentExpression) {
    // if the first expression ina chain is a cast, we may want to do a built-in operation on it
    if (parentExpression == nullptr)
        return expressionCast->getValueType();

    bool isSourceNumeric = parentExpression->getValueType()->isNumeric();

    bool areNumeric = parentExpression->getValueType()->isNumeric() && expressionCast->getValueType()->isNumeric();
    bool areBool = parentExpression->getValueType()->isBool() && expressionCast->getValueType()->isBool();
    bool areDataNumeric = parentExpression->getValueType()->isDataNumeric() && expressionCast->getValueType()->isDataNumeric();
    bool areDataBool = parentExpression->getValueType()->isDataBool() && expressionCast->getValueType()->isDataBool();

    if (areNumeric || areBool || areDataNumeric || areDataBool) {
        return expressionCast->getValueType();
    } else {
        markErrorInvalidCast(expressionCast->getLine(), expressionCast->getColumn(), parentExpression->getValueType(), expressionCast->getValueType());
        return nullptr;
    }
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionChained> expressionChained) {
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

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral) {
    vector<shared_ptr<ValueType>> elementTypes;
    for (shared_ptr<Expression> expression : expressionCompositeLiteral->getExpressions()) {
        shared_ptr<ValueType> elementType = typeForExpression(expression, nullptr, nullptr);
        if (elementType == nullptr)
            return nullptr;
        elementTypes.push_back(elementType);
    }
    return ValueType::composite(elementTypes);
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionGrouping> expressionGrouping) {
    expressionGrouping->valueType = typeForExpression(expressionGrouping->getSubExpression(), nullptr, nullptr);
    return expressionGrouping->getValueType();
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionIfElse> expressionIfElse, shared_ptr<ValueType> returnType) {
    // first check that condition is as BOOL
    expressionIfElse->conditionExpression = checkAndTryCasting(expressionIfElse->getConditionExpression(), ValueType::BOOL, returnType);
    shared_ptr<ValueType> conditionType = expressionIfElse->getConditionExpression()->getValueType();
    if (conditionType == nullptr) {
        return nullptr;
    } else if (!conditionType->isEqual(ValueType::BOOL)) {
        markErrorInvalidType(
            expressionIfElse->getConditionExpression()->getLine(),
            expressionIfElse->getConditionExpression()->getColumn(),
            conditionType, ValueType::BOOL
        );
    }

    // try corss-casting if else is present
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

shared_ptr<ValueType> TypesAnalyzer::TypesAnalyzer::typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral) {
    switch (expressionLiteral->getLiteralKind()) {
        case LiteralKind::BOOL:
            return ValueType::BOOL;
        case LiteralKind::INT:
            return ValueType::INT;
        case LiteralKind::FLOAT:
            return ValueType::FLOAT;
    }
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    ExpressionUnaryOperation operation = expressionUnary->getOperation();
    shared_ptr<ValueType> subType = typeForExpression(expressionUnary->getSubExpression(), nullptr, nullptr);

    if (!isUnaryOperationValidForType(expressionUnary->getOperation(), subType)) {
        markErrorInvalidOperationUnary(expressionUnary->getLine(), expressionUnary->getColumn(), operation, subType);
        return nullptr;
    }
    
    expressionUnary->valueType = typeForUnaryOperation(operation, subType);

    return expressionUnary->getValueType();
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionValue> expressionValue, shared_ptr<Expression> parentExpression) {
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
                    break;
                default:
                    expressionValue->valueType = nullptr;
                    markErrorInvalidBuiltIn(
                        expressionValue->getLine(),
                        expressionValue->getColumn(),
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
            markErrorInvalidBuiltIn(expressionValue->getLine(), expressionValue->getColumn(), expressionValue->getIdentifier(), parentExpression->getValueType());
            expressionValue->valueType = nullptr;
            return expressionValue->getValueType();
        // check blob member
        } else if (isBlob) {
            string blobName = *(parentExpression->getValueType()->getBlobName());
            optional<vector<pair<string, shared_ptr<ValueType>>>> blobMembers = scope->getBlobMembers(blobName);
            if (blobMembers) {
                for (pair<string, shared_ptr<ValueType>> &blobMember : *blobMembers) {
                    if (expressionValue->getIdentifier().compare(blobMember.first) == 0) {
                        expressionValue->valueType = blobMember.second;
                        expressionValue->valueKind = ExpressionValueKind::SIMPLE;
                        return expressionValue->getValueType();
                    }
                }
            }
            markErrorNotDefined(
                expressionValue->getLine(),
                expressionValue->getColumn(),
                format("{}.{}", blobName, expressionValue->getIdentifier())
            );
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
        if (!indexExpression->getValueType()->isInteger())
            markErrorInvalidType(indexExpression->getLine(), indexExpression->getColumn(), indexExpression->getValueType(), ValueType::INT);
        type = type->getSubType();
        expressionValue->valueKind = ExpressionValueKind::DATA;
    // finally check if it's a function
    } else if (type == nullptr) {
        type = scope->getFunctionType(expressionValue->getIdentifier());
        if (type != nullptr)
            expressionValue->valueKind = ExpressionValueKind::FUN;
    }

    if (type == nullptr)
        markErrorNotDefined(expressionValue->getLine(), expressionValue->getColumn(), expressionValue->getIdentifier());

    expressionValue->valueType = type;
    return expressionValue->getValueType();
}

//
// Support
//
bool TypesAnalyzer::isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
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

bool TypesAnalyzer::isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
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

shared_ptr<ValueType> TypesAnalyzer::typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
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

 shared_ptr<ValueType> TypesAnalyzer::typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
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

shared_ptr<Expression> TypesAnalyzer::checkAndTryCasting(shared_ptr<Expression> sourceExpression, shared_ptr<ValueType> targetType, shared_ptr<ValueType> returnType) {
    shared_ptr<ValueType> sourceType = typeForExpression(sourceExpression, nullptr, returnType);
    sourceExpression->valueType = sourceType;
    if (sourceType == nullptr || sourceType->isEqual(targetType))
        return sourceExpression;

    if (!canCast(sourceType, targetType))
        return sourceExpression;

    if (sourceExpression->getKind() == ExpressionKind::LITERAL || sourceExpression->getKind() == ExpressionKind::COMPOSITE_LITERAL) {
        sourceExpression->valueType = targetType;
        return sourceExpression;
    }

    shared_ptr<ExpressionChained> targetExpression = make_shared<ExpressionChained>(
        vector<shared_ptr<Expression>>(
            {
                sourceExpression,
                make_shared<ExpressionCast>(targetType, sourceExpression->getLine(), sourceExpression->getColumn())
            }
        ),
        sourceExpression->getLine(),
        sourceExpression->getColumn()
    );

    targetExpression->valueType = targetType;
    return targetExpression;
}

bool TypesAnalyzer::canCast(shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
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
                case ValueTypeKind::BLOB:
                    return true; // TODO: implement check
                case ValueTypeKind::DATA: {
                    vector<shared_ptr<ValueType>> sourceElementTypes = *(sourceType->getCompositeElementTypes());
                    for (shared_ptr<ValueType> sourceElementType : sourceElementTypes) {
                        if (!canCast(sourceElementType, targetType->getSubType()))
                            return false;
                    }
                    return true;
                }
                case ValueTypeKind::PTR:
                    return true; // TODO implement check

                default:
                    return false;
            }
            break;
        }

        default:
            return false;
    }
}

void TypesAnalyzer::markErrorInvalidType(int line, int column, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType) {
    string message;
    if (expectedType != nullptr)
        message = format("Invalid type {}, expected {}", Logger::toString(actualType), Logger::toString(expectedType));
    else
        message = format("Invalid type {}", Logger::toString(actualType));
    errors.push_back(Error::error(line, column, message));
}

void TypesAnalyzer::markErrorInvalidOperationUnary(int line, int column, ExpressionUnaryOperation operation, shared_ptr<ValueType> type) {
    string message = format("Invalid unary operation {} for type {}", Logger::toString(operation), Logger::toString(type));
    errors.push_back(Error::error(line, column, message));
}

void TypesAnalyzer::markErrorInvalidOperationBinary(int line, int column, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType) {
    string message = format("Invalid binary operation {} for types {} and {}", Logger::toString(operation), Logger::toString(firstType), Logger::toString(secondType));
    errors.push_back(Error::error(line, column, message));
}

void TypesAnalyzer::markErrorAlreadyDefined(int line, int column, string identifier) {
    string message = format("\"{}\" is already defined", identifier);
    errors.push_back(Error::error(line, column, message));
}

void TypesAnalyzer::markErrorNotDefined(int line, int column, string identifier) {
    string message = format("\"{}\" not defined", identifier);
    errors.push_back(Error::error(line, column, message));
}

void TypesAnalyzer::markErrorInvalidArgumentsCount(int line, int column, int actualCount, int expectedCount) {
    string message = format("Invalid arguments count {}, expected {}", actualCount, expectedCount);
    errors.push_back(Error::error(line, column, message));
}

 void TypesAnalyzer::markErrorInvalidCast(int line, int column, shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType) {
    string message = format("Invalid cast from {} to {}", Logger::toString(sourceType), Logger::toString(targetType));
    errors.push_back(Error::error(line, column, message));
 }

 void TypesAnalyzer::markErrorInvalidBuiltIn(int line, int column, string builtInName, shared_ptr<ValueType> type) {
    string message = format("Invalid built-in \"{}\" on type {}", builtInName, Logger::toString(type));
    errors.push_back(Error::error(line, column, message));
 }

 void TypesAnalyzer::markErrorInvalidImport(int line, int column, string moduleName) {
    string message = format("Invalid import, module \"{}\" doesn't exist", moduleName);
    errors.push_back(Error::error(line, column, message));
 }