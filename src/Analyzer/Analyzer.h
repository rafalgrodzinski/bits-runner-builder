#ifndef TYPES_ANALYZER_H
#define TYPES_ANALYZER_H

#include <format>
#include <map>
#include <memory>
#include <vector>

class AnalyzerScope;
class Module;
class Error;
class Location;

class Statement;
class StatementAssignment;
class StatementBlob;
class StatementBlobDeclaration;
class StatementBlock;
class StatementEnum;
class StatementExpression;
class StatementFunction;
class StatementFunctionDeclaration;
class StatementMetaExternFunction;
class StatementMetaExternVariable;
class StatementMetaImport;
class StatementModule;
class StatementProto;
class StatementProtoDeclaration;
class StatementRawFunction;
class StatementRepeat;
class StatementReturn;
class StatementVariable;
class StatementVariableDeclaration;

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

class ValueType;
class ValueTypeBlob;
class ValueTypeBoxed;
class ValueTypeData;
class ValueTypeEnum;
class ValueTypeEnumField;
class ValueTypeFun;
class ValueTypePtr;

enum class ExpressionUnaryOperation;
enum class ExpressionBinaryOperation;

class Analyzer {
private:
    enum class ImportLevel {
        NONE,
        EXPLICIT,
        IMPLICIT
    };

public:
    Analyzer(
        const std::string &defaultModuleName,
        std::shared_ptr<Module> module,
        const std::map<std::string, std::vector<std::shared_ptr<Statement>>> &importableHeaderStatementsMap
    );

    void checkModule();

private:
    std::vector<std::shared_ptr<Error>> errors;
    std::string defaultModuleName;

    std::shared_ptr<AnalyzerScope> scope;
    std::shared_ptr<Module> module;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> importableHeaderStatementsMap;
    std::map<std::string, ImportLevel> importedModuleLevelsMap;

    void checkStatement(std::shared_ptr<Statement> statement, std::shared_ptr<ValueType> returnType, bool isImported = false, ImportLevel importLevel = ImportLevel::NONE);
    void checkStatement(std::shared_ptr<StatementAssignment> statementAssignment);
    void checkStatement(std::shared_ptr<StatementBlob> statementBlob, bool isImported);
    void checkStatement(std::shared_ptr<StatementBlobDeclaration> statementBlobDeclaration);
    void checkStatement(std::shared_ptr<StatementBlock> statementBlock, std::shared_ptr<ValueType> returnType);
    void checkStatement(std::shared_ptr<StatementEnum> statementEnum);
    void checkStatement(std::shared_ptr<StatementExpression> statementExpression, std::shared_ptr<ValueType> returnType);
    void checkStatement(std::shared_ptr<StatementFunction> statementFunction);
    void checkStatement(std::shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration);
    void checkStatement(std::shared_ptr<StatementMetaExternFunction> statementMetaExternFunction);
    void checkStatement(std::shared_ptr<StatementMetaExternVariable> statementMetaExternVariable);
    void checkStatement(std::shared_ptr<StatementMetaImport> statement, ImportLevel ImportLevel);
    void checkStatement(std::shared_ptr<StatementProto> statement);
    void checkStatement(std::shared_ptr<StatementProtoDeclaration> statement);
    void checkStatement(std::shared_ptr<StatementRawFunction> statementRawFunction);
    void checkStatement(std::shared_ptr<StatementRepeat> statementRepeat, std::shared_ptr<ValueType> returnType);
    void checkStatement(std::shared_ptr<StatementReturn> statementReturn, std::shared_ptr<ValueType> returnType);
    void checkStatement(std::shared_ptr<StatementVariable> statementVariable);
    void checkStatement(std::shared_ptr<StatementVariableDeclaration> statementVariableDeclaration);

    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<Expression> expression, std::shared_ptr<Expression> parentExpression, std::shared_ptr<ValueType> returnType);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionBinary> expressionBinary);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionBlock> expressionBlock, std::shared_ptr<ValueType> returnType);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionCall> expressionCall, std::shared_ptr<Expression> parentExpression);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionCast> expressionCast, std::shared_ptr<Expression> parentExpression);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionChained> expressionChained);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionGrouping> expressionGrouping);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionIfElse> expressionIfElse, std::shared_ptr<ValueType> returnType);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionLiteral> expressionLiteral);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionUnary> expressionUnary);
    std::shared_ptr<ValueType> typeForExpression(std::shared_ptr<ExpressionValue> expressionValue, std::shared_ptr<Expression> parentExpression);

    //
    // Support
    //
    bool isUnaryOperationValidForType(ExpressionUnaryOperation operation, std::shared_ptr<ValueType> type) const;
    bool isBinaryOperationValidForTypes(ExpressionBinaryOperation operation, std::shared_ptr<ValueType> firstType, std::shared_ptr<ValueType> secondType) const;

    std::shared_ptr<ValueType> typeForUnaryOperation(ExpressionUnaryOperation operation, std::shared_ptr<ValueType> type) const;
    std::shared_ptr<ValueType> typeForBinaryOperation(ExpressionBinaryOperation operation, std::shared_ptr<ValueType> firstType, std::shared_ptr<ValueType> secondType) const;

    std::shared_ptr<Expression> checkAndTryCasting(std::shared_ptr<Expression> sourceExpression, std::shared_ptr<ValueType> targetType, std::shared_ptr<ValueType> returnType);
    bool canImplicitCast(std::shared_ptr<ValueType> sourceType, std::shared_ptr<ValueType> targetType);

    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueType> valueType, bool isCountExperssionRequired);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypeBlob> valueTypeBlob);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypeBoxed> valueTypeBoxed);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypeData> valueTypeData, bool isCountExperssionRequired);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypeEnum> valueTypeEnum);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypeEnumField> valueTypeEnumField);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypeFun> valueTypeFun);
    std::shared_ptr<ValueType> typeForCheckedValueType(std::shared_ptr<ValueTypePtr> valueTypePtr);

    void markErrorAlreadyDefined(std::shared_ptr<Location> location, const std::string &identifier);
    void markErrorInvalidAttribute(std::shared_ptr<Location> location, const std::string &name);
    void markErrorInvalidArgumentsCount(std::shared_ptr<Location> location, int actulCount, int expectedCount);
    void markErrorInvalidBuiltIn(std::shared_ptr<Location> location, const std::string &builtInName, std::shared_ptr<ValueType> type);
    void markErrorInvalidCast(std::shared_ptr<Location> location, std::shared_ptr<ValueType> sourceType, std::shared_ptr<ValueType> targetType);
    void markErrorInvalidImport(std::shared_ptr<Location> location, const std::string &moduleName);
    void markErrorInvalidName(std::shared_ptr<Location> location, const std::string &name);
    void markErrorInvalidOperationBinary(std::shared_ptr<Location> location, ExpressionBinaryOperation operation, std::shared_ptr<ValueType> firstType, std::shared_ptr<ValueType> secondType);
    void markErrorInvalidOperationUnary(std::shared_ptr<Location> location, ExpressionUnaryOperation operation, std::shared_ptr<ValueType> type);
    void markErrorInvalidType(std::shared_ptr<Location> location, std::shared_ptr<ValueType> actualType, std::shared_ptr<ValueType> expectedType);
    void markErrorNotDefined(std::shared_ptr<Location> location, const std::string &name);
    void markErrorNotImplemented(std::shared_ptr<Location> location, const std::string &protoName, const std::string &memberName);
    void markErrorUnexpectedExpression(std::shared_ptr<Location> location);
};

#endif