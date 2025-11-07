#include "TypesAnalyzer.h"

#include "Error.h"
#include "Logger.h"

#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionBinary.h"
#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Expression/ExpressionUnary.h"

#include "Parser/Statement/Statement.h"
#include "Parser/Statement/StatementBlock.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementFunction.h"
#include "Parser/Statement/StatementModule.h"
#include "Parser/Statement/StatementReturn.h"

#include "Parser/ValueType.h"

void TypesAnalyzer::checkModule(shared_ptr<StatementModule> module) {
    for (shared_ptr<Statement> statement : module->getStatements())
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
        case StatementKind::MODULE:
            checkStatement(dynamic_pointer_cast<StatementModule>(statement));
            break;
        case StatementKind::FUNCTION:
            checkStatement(dynamic_pointer_cast<StatementFunction>(statement));
            break;
        case StatementKind::BLOCK:
            checkStatement(dynamic_pointer_cast<StatementBlock>(statement), returnType);
            break;
        case StatementKind::RETURN:
            checkStatement(dynamic_pointer_cast<StatementReturn>(statement), returnType);
            break;
        case StatementKind::EXPRESSION:
            checkStatement(dynamic_pointer_cast<StatementExpression>(statement));
            break;
    }
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementModule> statementModule) {
    checkStatement(statementModule, nullptr);
}


void TypesAnalyzer::checkStatement(shared_ptr<StatementFunction> statementFunction) {
    checkStatement(statementFunction->getStatementBlock(), statementFunction->getReturnValueType());
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType) {
    for (shared_ptr<Statement> statement : statementBlock->getStatements())
        checkStatement(statement, returnType);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType) {
    shared_ptr<ValueType> expressionType = typeForExpression(statementReturn->getExpression());

    if (expressionType != returnType)
        markError(statementReturn->getExpression()->getLine(), statementReturn->getExpression()->getColumn(), expressionType, returnType);
}

void TypesAnalyzer::checkStatement(shared_ptr<StatementExpression> statementExpression) {

}

//
// Expressions
//
shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case ExpressionKind::LITERAL:
            return typeForExpression(dynamic_pointer_cast<ExpressionLiteral>(expression));
        case ExpressionKind::UNARY:
            return typeForExpression(dynamic_pointer_cast<ExpressionUnary>(expression));
        case ExpressionKind::BINARY:
            return typeForExpression(dynamic_pointer_cast<ExpressionBinary>(expression));
    }
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral) {
    return expressionLiteral->getValueType();
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionUnary> expressionUnary) {
    ExpressionUnaryOperation operation = expressionUnary->getOperation();
    shared_ptr<ValueType> subType = typeForExpression(expressionUnary->getSubExpression());

    if (!isUnaryOperationValidForType(expressionUnary->getOperation(), subType)) {
        markErrorInvalidOperationUnary(expressionUnary->getLine(), expressionUnary->getColumn(), subType, operation);
        return nullptr;
    }
    
    expressionUnary->valueType = typeForUnaryOperation(operation, subType);

    return expressionUnary->getValueType();
}

shared_ptr<ValueType> TypesAnalyzer::typeForExpression(shared_ptr<ExpressionBinary> expressionBinary) {
    ExpressionBinaryOperation operation = expressionBinary->getOperation();
    shared_ptr<ValueType> firstType = typeForExpression(expressionBinary->getLeft());
    shared_ptr<ValueType> secondType = typeForExpression(expressionBinary->getRight());

    // types must match
    if (!isBinaryOperationValidForTypes(expressionBinary->getOperation(), firstType, secondType)) {
        markErrorInvalidOperationBinary(expressionBinary->getLine(), expressionBinary->getColumn(), firstType, secondType, operation);
        return nullptr;
    }

    expressionBinary->valueType = typeForBinaryOperation(operation, firstType, secondType);

    return expressionBinary->getValueType();
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
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
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
    bool areTypesMatcing = firstType == secondType;

    switch (firstType->getKind()) {
        // Valid operations for boolean types
        case ValueTypeKind::BOOL: {
            switch (operation) {
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
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
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
                        case ValueTypeKind::UINT:
                        case ValueTypeKind::U8:
                        case ValueTypeKind::U32:
                        case ValueTypeKind::U64:

                        case ValueTypeKind::SINT:
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
                case ValueTypeKind::UINT:
                    return ValueType::SINT;
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

void TypesAnalyzer::markError(int line, int column, shared_ptr<ValueType> expectedType, shared_ptr<ValueType> actualType) {
    errors.push_back(Error::analyzerTypeError(line, column, expectedType, actualType));
}

void TypesAnalyzer::markErrorInvalidOperationUnary(int line, int column, shared_ptr<ValueType> type, ExpressionUnaryOperation operation) {
    errors.push_back(Error::analyzerTypeInvalidOperationUnary(line, column, type, operation));
}

void TypesAnalyzer::markErrorInvalidOperationBinary(int line, int column, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType, ExpressionBinaryOperation operation) {
    errors.push_back(Error::analyzerTypeInvalidOperationBinary(line, column, firstType, secondType, operation));
}
