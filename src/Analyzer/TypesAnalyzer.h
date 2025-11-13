#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <memory>
#include <vector>
#include <map>

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
class ExpressionValue;

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
    vector<shared_ptr<Statement>> statements;
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap;
    string importModulePrefix;

    void checkStatement(shared_ptr<Statement> statement, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementAssignment> statementAssignment);
    void checkStatement(shared_ptr<StatementBlock> statementBlock, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementExpression> statementExpression, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementFunction> statementFunction);
    void checkStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration);
    void checkStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction);
    void checkStatement(shared_ptr<StatementMetaImport> StatementMetaImport);
    void checkStatement(shared_ptr<StatementRepeat> statementRepeat, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementReturn> statementReturn, shared_ptr<ValueType> returnType);
    void checkStatement(shared_ptr<StatementVariable> statementVariable);
    void checkStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration);

    shared_ptr<ValueType> typeForExpression(shared_ptr<Expression> expression, shared_ptr<Expression> parentExpression, shared_ptr<ValueType> returnType);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionBinary> expressionBinary);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionBlock> expressionBlock, shared_ptr<ValueType> returnType);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionCall> expressionCall);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionCast> expressionCast, shared_ptr<Expression> parentExpression);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionChained> expressionChained);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionGrouping> expressionGrouping);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionIfElse> expressionIfElse, shared_ptr<ValueType> returnType);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionLiteral> expressionLiteral);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionUnary> expressionUnary);
    shared_ptr<ValueType> typeForExpression(shared_ptr<ExpressionValue> expressionValue, shared_ptr<Expression> parentExpression);

    //
    // Support
    //
    bool isUnaryOperationValidForType(ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    bool isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);

    shared_ptr<ValueType> typeForUnaryOperation(ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    shared_ptr<ValueType> typeForBinaryOperation(ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);

    shared_ptr<Expression> checkAndTryCasting(shared_ptr<Expression> sourceExpression, shared_ptr<ValueType> targetType, shared_ptr<ValueType> returnType);
    bool canCast(shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType);

    void markErrorInvalidType(int line, int column, shared_ptr<ValueType> actualType, shared_ptr<ValueType> expectedType);
    void markErrorInvalidOperationUnary(int line, int column, ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    void markErrorInvalidOperationBinary(int line, int column, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);
    void markErrorAlreadyDefined(int line, int column, string identifier);
    void markErrorNotDefined(int line, int column, string identifier);
    void markErrorInvalidArgumentsCount(int line, int column, int actulCount, int expectedCount);
    void markErrorInvalidCast(int line, int column, shared_ptr<ValueType> sourceType, shared_ptr<ValueType> targetType);
    void markErrorInvalidBuiltIn(int line, int column, string builtInName, shared_ptr<ValueType> type);
    void markErrorInvalidImport(int line, int column, string moduleName);

public:
    TypesAnalyzer(
        vector<shared_ptr<Statement>> statements,
        map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap
    );
    void checkModule();
};

#endif