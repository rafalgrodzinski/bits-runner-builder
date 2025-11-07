#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <memory>
#include <vector>

class Error;

class Expression;
class ExpressionBinary;
class ExpressionLiteral;
class ExpressionUnary;

class Statement;
class StatementBlock;
class StatementExpression;
class StatementFunction;
class StatementModule;
class StatementReturn;
class ValueType;

enum class ExpressionUnaryOperation;
enum class ExpressionBinaryOperation;

using namespace std;

class TypesAnalyzer {
private:
    vector<shared_ptr<Error>> errors;

    void checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementModule> statementModule);
    void checkStatement(shared_ptr<StatementFunction> statementFunction);
    void checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementExpression> statementExpression);

    shared_ptr<ValueType> typeForExpression(shared_ptr<Expression> expression);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionUnary> expressionUnary);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionBinary> expressionBinary);

    //
    // Support
    //
    bool isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    bool isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);

    shared_ptr<ValueType> typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    shared_ptr<ValueType> typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);

    void markError(int line, int column, shared_ptr<ValueType> expectedType, shared_ptr<ValueType> actualType);
    void markErrorInvalidOperationUnary(int line, int column, shared_ptr<ValueType> type, ExpressionUnaryOperation operation);
    void markErrorInvalidOperationBinary(int line, int column, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType, ExpressionBinaryOperation operation);

public:
    void checkModule(shared_ptr<StatementModule> module);
};

#endif