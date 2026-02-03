#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include <format>
#include <map>
#include <stack>
#include <ranges>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/NVPTXAddrSpace.h>
#include <llvm/Support/Error.h>

#include "Scope.h"

class Error;
class Location;
class Module;
class ValueType;
class WrappedValue;

class Statement;
class StatementAssignment;
class StatementBlob;
class StatementBlobDeclaration;
class StatementBlock;
class StatementExpression;
class StatementFunction;
class StatementFunctionDeclaration;
class StatementMetaExternFunction;
class StatementMetaExternVariable;
class StatementMetaImport;
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

enum class ExpressionBinaryOperation;
enum class ExpressionUnaryOperation;

using namespace std;

class ModuleBuilder {
private:
    vector<shared_ptr<Error>> errors;
    string defaultModuleName;

    shared_ptr<Module> module;
    map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap;

    shared_ptr<Scope> scope;

    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> moduleLLVM;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::CallingConv::ID callingConvention;

    llvm::Type *typeVoid;
    llvm::Type *typeBool;

    llvm::IntegerType *typeInt;
    llvm::IntegerType *typeI8;
    llvm::IntegerType *typeI16;
    llvm::IntegerType *typeI32;
    llvm::IntegerType *typeI64;

    llvm::Type *typeF32;
    llvm::Type *typeF64;
    llvm::Type *typeFloat;

    llvm::Type *typePtr;
    llvm::IntegerType *typeA;

    // Statements
    void buildStatement(shared_ptr<Statement> statement);
    void buildStatement(shared_ptr<StatementAssignment> statementAssignment);
    void buildStatement(shared_ptr<StatementBlob> statementBlob);
    void buildStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration);
    void buildStatement(shared_ptr<StatementBlock> statementBlock);
    void buildStatement(shared_ptr<StatementExpression> statementExpression);
    void buildStatement(shared_ptr<StatementFunction> statementFunction);
    void buildStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration);
    void buildStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction);
    void buildStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable);
    void buildStatement(shared_ptr<StatementMetaImport> statementMetaImport);
    void buildStatement(shared_ptr<StatementRawFunction> statementRawFunction);
    void buildStatement(shared_ptr<StatementRepeat> statementRepeat);
    void buildStatement(shared_ptr<StatementReturn> statementReturn);
    void buildStatement(shared_ptr<StatementVariable> statementVariable);
    void buildStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration);

    void buildFunctionDeclaration(string moduleName, string name, bool isExtern, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnType);
    void buildRawFunction(string moduleName, shared_ptr<StatementRawFunction> statement);
    void buildVariableDeclaration(string moduleName, string name, bool isExtern, shared_ptr<ValueType> valueType);
    void buildBlobDeclaration(string moduleName, string name);
    void buildBlobDefinition(string moduleName, string name, vector<pair<string, shared_ptr<ValueType>>> members);
    void buildLocalVariable(shared_ptr<StatementVariable> statement);
    void buildGlobalVariable(shared_ptr<StatementVariable> statement);
    void buildAssignment(shared_ptr<WrappedValue> targetWrappedValue, shared_ptr<Expression> valueExpression);

    // Expressions
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<Expression> expression);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionBinary> expressionBinary);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionBlock> expressionBlock);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionCall> expressionCall);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionChained> expressionChained);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionGrouping> expressionGrouping);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionIfElse> expressionIfElse);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionLiteral> expressionLiteral);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionUnary> expressionUnary);
    shared_ptr<WrappedValue> wrappedValueForExpression(shared_ptr<ExpressionValue> expressionValue);

    shared_ptr<WrappedValue> wrappedValueForBuiltIn(shared_ptr<WrappedValue> parentWrappedValue, shared_ptr<Expression> parentExpression, shared_ptr<Expression> expression);
    shared_ptr<WrappedValue> wrappedValueForCall(llvm::Value *callee, llvm::FunctionType *funType, vector<llvm::Value*> implicitArguments, vector<shared_ptr<Expression>> argumentExpressions, shared_ptr<ValueType> valueType);
    shared_ptr<WrappedValue> wrappedValueForCast(shared_ptr<WrappedValue> wrappedValue, shared_ptr<ValueType> targetValueType);
    shared_ptr<WrappedValue> wrappedValueForSourceValue(llvm::Value *sourceValue, llvm::Type *sourceType,  shared_ptr<Expression> expression);
    shared_ptr<WrappedValue> wrappedValueForTypeBuiltIn(llvm::Type *type, shared_ptr<ExpressionValue> expression);

    // Support
    llvm::Type *typeForValueType(shared_ptr<ValueType> valueType, shared_ptr<Location> location = nullptr);
    int sizeInBitsForType(llvm::Type *type);

    // Error Handling    
    void markFunctionError(string name, string message);
    void markModuleError(string message);
    
    void markErrorAlreadyDefined(shared_ptr<Location> location, string name);
    void markInvalidConstraints(shared_ptr<Location> location, string functionName, string constraints);
    void markErrorInvalidAssignment(shared_ptr<Location> location);
    void markErrorInvalidBuiltIn(shared_ptr<Location> location, string name);
    void markErrorInvalidCast(shared_ptr<Location> location);
    void markErrorInvalidConstant(shared_ptr<Location> location);
    void markErrorInvalidGlobal(shared_ptr<Location> location);
    void markErrorInvalidImport(shared_ptr<Location> location, string moduleName);
    void markErrorInvalidLiteral(shared_ptr<Location> location, shared_ptr<ValueType> type);
    void markErrorInvalidMember(shared_ptr<Location> location, string blobName, string memberName);
    void markErrorInvalidOperationBinary(shared_ptr<Location> location, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);
    void markErrorInvalidOperationUnary(shared_ptr<Location> location, ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    void markErrorInvalidType(shared_ptr<Location> location);
    void markErrorUnexpected(shared_ptr<Location> location, string name);
    void markErrorNotDeclared(shared_ptr<Location> location, string name);
    void markErrorNotDefined(shared_ptr<Location> location, string name);
    void markErrorNoTypeForPointer(shared_ptr<Location> location);

    void debugPrint(vector<llvm::Value *> values);

public:
    ModuleBuilder(
        string defaultModuleName,
        int intSize,
        int pointerSize,
        llvm::CallingConv::ID callingConvention,
        shared_ptr<Module> module,
        map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap
    );
    shared_ptr<llvm::Module> getModuleLLVM();
};

#endif