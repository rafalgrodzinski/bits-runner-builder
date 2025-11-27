#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include <map>
#include <stack>

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
class ValueType;

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
    string moduleName;
    string defaultModuleName;

    vector<shared_ptr<Statement>> statements;
    vector<shared_ptr<Statement>> headerStatements;
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap;

    shared_ptr<Scope> scope;

    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::CallingConv::ID callingConvention;

    llvm::Type *typeVoid;
    llvm::Type *typeBool;
    
    llvm::IntegerType *typeU8;
    llvm::IntegerType *typeU32;
    llvm::IntegerType *typeU64;
    llvm::IntegerType *typeUInt;

    llvm::IntegerType *typeS8;
    llvm::IntegerType *typeS32;
    llvm::IntegerType *typeS64;
    llvm::IntegerType *typeSInt;

    llvm::Type *typeF32;
    llvm::Type *typeF64;
    llvm::Type *typeFloat;

    llvm::Type *typePtr;
    llvm::IntegerType *typeIntPtr;

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
    void buildVariableDeclaration(string moduleName, string name, bool isExtern, shared_ptr<ValueType> valueType);
    void buildLocalVariable(shared_ptr<StatementVariable> statement);
    void buildGlobalVariable(shared_ptr<StatementVariable> statement);
    void buildAssignment(llvm::Value *targetValue, llvm::Type *targetType, shared_ptr<Expression> valueExpression);

    // Expressions
    llvm::Value *valueForExpression(shared_ptr<Expression> expression, llvm::Type *castToType = nullptr);
    llvm::Value *valueForExpression(shared_ptr<ExpressionBinary> expressionBinary);
    llvm::Value *valueForExpression(shared_ptr<ExpressionBlock> expressionBlock);
    llvm::Value *valueForExpression(shared_ptr<ExpressionCall> expressionCall);
    llvm::Value *valueForExpression(shared_ptr<ExpressionChained> expressionChained);
    llvm::Value *valueForExpression(shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral, llvm::Type *castToType = nullptr);
    llvm::Value *valueForExpression(shared_ptr<ExpressionGrouping> expressionGrouping);
    llvm::Value *valueForExpression(shared_ptr<ExpressionIfElse> expressionIfElse);
    llvm::Value *valueForExpression(shared_ptr<ExpressionLiteral> expressionLiteral, llvm::Type *castToType = nullptr);
    llvm::Value *valueForExpression(shared_ptr<ExpressionUnary> expressionUnary);
    llvm::Value *valueForExpression(shared_ptr<ExpressionValue> expressionValue);

    llvm::Constant *constantValueForExpression(shared_ptr<Expression> expression, llvm::Type *targetType);
    llvm::Constant *constantValueForCompositeLiteral(shared_ptr<ExpressionCompositeLiteral> expression, llvm::Type *castToType);
    llvm::Value *valueForCall(llvm::Value *fun, llvm::FunctionType *funType, shared_ptr<ExpressionCall> expression);
    
    llvm::Value *valueForSourceValue(llvm::Value *sourceValue, llvm::Type *sourceType,  shared_ptr<Expression> expression);
    llvm::Value *valueForBuiltIn(llvm::Value *parentValue, shared_ptr<ExpressionValue> parentExpression, shared_ptr<Expression> expression);
    llvm::Value *valueForTypeBuiltIn(llvm::Type *type, shared_ptr<ExpressionValue> expression);
    llvm::Value *valueForCast(llvm::Value *sourceValue, shared_ptr<ValueType> targetValueType);

    // Support
    llvm::Type *typeForValueType(shared_ptr<ValueType> valueType, int count = 0);
    int sizeInBitsForType(llvm::Type *type);

    void markError(int line, int column, string message);
    void markFunctionError(string name, string message);
    void markModuleError(string message);
    void markErrorInvalidOperationBinary(int line, int column, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);
    void markErrorInvalidOperationUnary(int line, int column, ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    void markErrorAlreadyDefined(int line, int column, string identifier);
    void markErrorNotDeclared(int line, int column, string identifier);
    void markErrorNotDefined(int line, int column, string identifier);
    void markInvalidConstraints(int line, int column, string functionName, string constraints);
    void markErrorInvalidLiteral(int line, int column, shared_ptr<ValueType> type);

public:
    ModuleBuilder(
        string moduleName,
        string defaultModuleName,
        int intSize,
        int pointerSize,
        llvm::CallingConv::ID callingConvention,
        vector<shared_ptr<Statement>> statements,
        vector<shared_ptr<Statement>> headerStatements,
        map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap
    );
    shared_ptr<llvm::Module> getModule();
};

#endif