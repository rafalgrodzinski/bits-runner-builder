#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <memory>
#include <vector>

class AnalyzerScope;
class Error;
class ValueType;

class Expression;
class ExpressionBinary;
class ExpressionBlock;
class ExpressionCall;
class ExpressionCast;
class ExpressionChained;
class ExpressionCompositeLiteral;
class ExpressionGrouping;
class ExpressionIfElse;
class ExpressionLiteral;
class ExpressionNone;
class ExpressionUnary;
class ExpressionVariable;

class Statement;
class StatementAssignment;
class StatementBlob;
class StatementBlobDeclaration;
class StatementBlock;
class StatementExpression;
class StatementFunction;
class StatementFunctionDeclaration;
class StatementMetaExternVariable;
class StatementMetaExternFunction;
class StatementMetaImport;
class StatementModule;
class StatementRawFunction;
class StatementRepeat;
class StatementReturn;
class StatementVariableDeclaration;
class StatementVariable;

enum class ExpressionUnaryOperation;
enum class ExpressionBinaryOperation;

using namespace std;

class TypesAnalyzer {
private:
    vector<shared_ptr<Error>> errors;
    shared_ptr<AnalyzerScope> scope;

    void checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementAssignment> statementAssignment);
    void checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementExpression> statementExpression, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementFunction> statementFunction);
    void checkStatement(shared_ptr<StatementModule> statementModule);
    void checkStatement(shared_ptr<StatementRepeat> statementRepeat, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementVariable> statementVariable);

    shared_ptr<ValueType> typeForExpression(shared_ptr<Expression> expression, shared_ptr<ValueType> returnType = nullptr);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionBinary> expressionBinary);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionBlock> expressionBlock, shared_ptr<ValueType> returnType);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionCall> expressionCall);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionCast> expressionCast);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionChained> expressionChained);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionGrouping> expressionGrouping);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionIfElse> expressionIfElse, shared_ptr<ValueType> returnType);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionUnary> expressionUnary);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionVariable> expressionVariable);

    //
    // Support
    //
    bool isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    bool isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);

    shared_ptr<ValueType> typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    shared_ptr<ValueType> typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);

    void markError(int line, int column, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType);
    void markErrorInvalidOperationUnary(int line, int column, shared_ptr<ValueType> type, ExpressionUnaryOperation operation);
    void markErrorInvalidOperationBinary(int line, int column, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType, ExpressionBinaryOperation operation);
    void markErrorAlreadyDefined(int line, int column, string identifier);
    void markErrorNotDefined(int line, int column, string identifier);
    void markErrorInvalidArgumentsCount(int line, int column, int actulCount, int expectedCount);

public:
    void checkModule(shared_ptr<StatementModule> module);
};

#endif