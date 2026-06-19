#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include <format>
#include <map>
#include <ranges>
#include <stack>

#include <llvm/IR/Constants.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Error.h>
#include <llvm/Target/TargetMachine.h>

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

enum class ExpressionBinaryOperation;
enum class ExpressionUnaryOperation;

using namespace std;

class ModuleBuilder {
    enum class ImportLevel {
        NONE,
        EXPLICIT,
        IMPLICIT
    };

private:
    vector<shared_ptr<Error>> errors;
    string defaultModuleName;

    shared_ptr<Module> module;
    map<string, vector<shared_ptr<Statement>>> importableHeaderStatementsMap;
    map<string, ImportLevel> importedModuleLevelsMap;

    shared_ptr<Scope> scope;

    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> llvmModule;
    shared_ptr<llvm::IRBuilder<>> builder;
    llvm::BasicBlock *currentInitBlock = nullptr;

    llvm::Triple::ArchType archType;
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
    llvm::IntegerType *typePtrInt;
    llvm::Type *typeBoxed;

    // Statements
    void buildStatement(shared_ptr<Statement> statement, ImportLevel importLevel = ImportLevel::NONE);
    void buildStatement(shared_ptr<StatementAssignment> statementAssignment);
    void buildStatement(shared_ptr<StatementBlob> statementBlob);
    void buildStatement(shared_ptr<StatementBlobDeclaration> statementBlobDeclaration);
    void buildStatement(shared_ptr<StatementBlock> statementBlock);
    void buildStatement(shared_ptr<StatementExpression> statementExpression);
    void buildStatement(shared_ptr<StatementFunction> statementFunction);
    void buildStatement(shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration);
    void buildStatement(shared_ptr<StatementMetaExternFunction> statementMetaExternFunction);
    void buildStatement(shared_ptr<StatementMetaExternVariable> statementMetaExternVariable);
    void buildStatement(shared_ptr<StatementMetaImport> statementMetaImport, ImportLevel importLevel);
    void buildStatement(shared_ptr<StatementProto> statementProto);
    void buildStatement(shared_ptr<StatementProtoDeclaration> statementProtoDeclaration);
    void buildStatement(shared_ptr<StatementRawFunction> statementRawFunction);
    void buildStatement(shared_ptr<StatementRepeat> statementRepeat);
    void buildStatement(shared_ptr<StatementReturn> statementReturn);
    void buildStatement(shared_ptr<StatementVariable> statementVariable);
    void buildStatement(shared_ptr<StatementVariableDeclaration> statementVariableDeclaration);

    void buildFunctionDeclaration(const string &moduleName, const string &name, bool shouldExport, const vector<pair<string, shared_ptr<ValueType>>> &arguments, shared_ptr<ValueType> returnType);
    void buildRawFunction(const string &moduleName, shared_ptr<StatementRawFunction> statement);
    void buildVariableDeclaration(const string &moduleName, const string &name, bool shouldExport, shared_ptr<ValueType> valueType);

    void buildProtoDeclaration(const string &moduleName, shared_ptr<StatementProtoDeclaration> statement);
    void buildProtoDefinition(const string &moduleName, shared_ptr<StatementProto> statement);

    void buildLocalVariable(shared_ptr<StatementVariable> statement);
    void buildGlobalVariable(shared_ptr<StatementVariable> statement);
    void buildAssignment(shared_ptr<WrappedValue> targetWrappedValue, shared_ptr<Expression> valueExpression);
    llvm::AllocaInst *buildAlloca(llvm::Type *type, const string &identifier = "");

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
    shared_ptr<WrappedValue> wrappedValueForCall(llvm::Value *callee, llvm::FunctionType *funType, const vector<llvm::Value*> &implicitArguments, const vector<shared_ptr<Expression>> &argumentExpressions, shared_ptr<ValueType> valueType);
    shared_ptr<WrappedValue> wrappedValueForCast(shared_ptr<WrappedValue> wrappedValue, shared_ptr<ValueType> targetValueType);
    shared_ptr<WrappedValue> wrappedValueForValue(llvm::Value *value, llvm::Value *pointerValue, llvm::Type *type, shared_ptr<Expression> expression);
    shared_ptr<WrappedValue> wrappedValueForTypeBuiltIn(llvm::Type *type, shared_ptr<ExpressionValue> expression);

    // Support
    llvm::Type *llvmTypeForValueType(shared_ptr<ValueType> valueType, bool shouldUnbox = false, shared_ptr<Location> location = nullptr);
    int sizeInBitsForType(llvm::Type *type);

    // Error Handling    
    void markFunctionError(const string &name, const string &message);
    void markModuleError(const string &message);
    
    void markErrorAlreadyDefined(shared_ptr<Location> location, const string &name);
    void markInvalidConstraints(shared_ptr<Location> location, const string &functionName, const string &constraints);
    void markErrorInvalidAssignment(shared_ptr<Location> location);
    void markErrorInvalidBuiltIn(shared_ptr<Location> location, const string &name);
    void markErrorInvalidCast(shared_ptr<Location> location);
    void markErrorInvalidConstant(shared_ptr<Location> location);
    void markErrorInvalidGlobal(shared_ptr<Location> location);
    void markErrorInvalidImport(shared_ptr<Location> location, const string &moduleName);
    void markErrorInvalidLiteral(shared_ptr<Location> location, shared_ptr<ValueType> type);
    void markErrorInvalidMember(shared_ptr<Location> location, const string &blobName, const string &memberName);
    void markErrorInvalidOperationBinary(shared_ptr<Location> location, ExpressionBinaryOperation operation, shared_ptr<ValueType> firstType, shared_ptr<ValueType> secondType);
    void markErrorInvalidOperationUnary(shared_ptr<Location> location, ExpressionUnaryOperation operation, shared_ptr<ValueType> type);
    void markErrorInvalidType(shared_ptr<Location> location);
    void markErrorUnexpected(shared_ptr<Location> location, const string &name);
    void markErrorNotDeclared(shared_ptr<Location> location, const string &name);
    void markErrorNotDefined(shared_ptr<Location> location, const string &name);
    void markErrorNoTypeForPointer(shared_ptr<Location> location);

    void debugPrint(const vector<llvm::Value *> &values);
    void debugPrint(const vector<llvm::Type *> &types);

public:
    ModuleBuilder(
        const string &defaultModuleName,
        llvm::Triple::ArchType archType,
        llvm::DataLayout dataLayout,
        llvm::CallingConv::ID callingConvention,
        shared_ptr<Module> module,
        const map<string, vector<shared_ptr<Statement>>> &importableHeaderStatementsMap
    );
    shared_ptr<llvm::Module> getLlvmModule();
};

#endif