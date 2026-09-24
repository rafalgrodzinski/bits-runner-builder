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
class StatementEnum;
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

class ModuleBuilder {
    enum class ImportLevel {
        NONE,
        EXPLICIT,
        IMPLICIT
    };

public:
    ModuleBuilder(
        const std::string &defaultModuleName,
        llvm::Triple::ArchType archType,
        llvm::DataLayout dataLayout,
        llvm::CallingConv::ID callingConvention,
        std::shared_ptr<Module> module,
        const std::map<std::string, std::vector<std::shared_ptr<Statement>>> &importableHeaderStatementsMap
    );
    std::shared_ptr<llvm::Module> getLlvmModule();

private:
    std::vector<std::shared_ptr<Error>> errors;
    std::string defaultModuleName;

    std::shared_ptr<Module> module;
    std::map<std::string, std::vector<std::shared_ptr<Statement>>> importableHeaderStatementsMap;
    std::map<std::string, ImportLevel> importedModuleLevelsMap;

    std::shared_ptr<Scope> scope;

    std::shared_ptr<llvm::LLVMContext> context;
    std::shared_ptr<llvm::Module> llvmModule;
    std::shared_ptr<llvm::IRBuilder<>> builder;
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

    llvm::StructType *typeEnumStruct;

    // Statements
    void buildStatement(std::shared_ptr<Statement> statement, ImportLevel importLevel = ImportLevel::NONE);
    void buildStatement(std::shared_ptr<StatementAssignment> statementAssignment);
    void buildStatement(std::shared_ptr<StatementBlob> statementBlob);
    void buildStatement(std::shared_ptr<StatementBlobDeclaration> statementBlobDeclaration);
    void buildStatement(std::shared_ptr<StatementBlock> statementBlock);
    void buildStatement(std::shared_ptr<StatementEnum> statementEnum);
    void buildStatement(std::shared_ptr<StatementExpression> statementExpression);
    void buildStatement(std::shared_ptr<StatementFunction> statementFunction);
    void buildStatement(std::shared_ptr<StatementFunctionDeclaration> statementFunctionDeclaration);
    void buildStatement(std::shared_ptr<StatementMetaExternFunction> statementMetaExternFunction);
    void buildStatement(std::shared_ptr<StatementMetaExternVariable> statementMetaExternVariable);
    void buildStatement(std::shared_ptr<StatementMetaImport> statementMetaImport, ImportLevel importLevel);
    void buildStatement(std::shared_ptr<StatementProto> statementProto);
    void buildStatement(std::shared_ptr<StatementProtoDeclaration> statementProtoDeclaration);
    void buildStatement(std::shared_ptr<StatementRawFunction> statementRawFunction);
    void buildStatement(std::shared_ptr<StatementRepeat> statementRepeat);
    void buildStatement(std::shared_ptr<StatementReturn> statementReturn);
    void buildStatement(std::shared_ptr<StatementVariable> statementVariable);
    void buildStatement(std::shared_ptr<StatementVariableDeclaration> statementVariableDeclaration);

    void buildLocalVariable(std::shared_ptr<StatementVariable> statement);
    void buildGlobalVariable(std::shared_ptr<StatementVariable> statement);
    void buildAssignment(std::shared_ptr<WrappedValue> targetWrappedValue, std::shared_ptr<Expression> valueExpression);
    llvm::AllocaInst *buildAlloca(llvm::Type *type, const std::string &identifier = "");

    // Expressions
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<Expression> expression);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionBinary> expressionBinary);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionBlock> expressionBlock);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionCall> expressionCall);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionChained> expressionChained);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteral);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionGrouping> expressionGrouping);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionIfElse> expressionIfElse);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionLiteral> expressionLiteral);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionUnary> expressionUnary);
    std::shared_ptr<WrappedValue> wrappedValueForExpression(std::shared_ptr<ExpressionValue> expressionValue);

    std::shared_ptr<WrappedValue> wrappedValueForBuiltIn(std::shared_ptr<WrappedValue> parentWrappedValue, std::shared_ptr<Expression> parentExpression, std::shared_ptr<Expression> expression);
    std::shared_ptr<WrappedValue> wrappedValueForCall(llvm::Value *callee, llvm::FunctionType *funType, const std::vector<llvm::Value*> &implicitArguments, const std::vector<std::shared_ptr<Expression>> &argumentExpressions, std::shared_ptr<ValueType> valueType);
    std::shared_ptr<WrappedValue> wrappedValueForCast(std::shared_ptr<WrappedValue> wrappedValue, std::shared_ptr<ValueType> targetValueType);
    std::shared_ptr<WrappedValue> wrappedValueForValue(llvm::Value *value, llvm::Value *pointerValue, llvm::Type *type, std::shared_ptr<Expression> expression);
    std::shared_ptr<WrappedValue> wrappedValueForTypeBuiltIn(llvm::Type *type, std::shared_ptr<ExpressionValue> expression);

    // Support
    llvm::Type *llvmTypeForValueType(std::shared_ptr<ValueType> valueType, bool shouldUnbox = false);
    int sizeInBitsForType(llvm::Type *type);

    // Error Handling    
    void markFunctionError(const std::string &name, const std::string &message);
    void markModuleError(const std::string &message);
    
    void markErrorAlreadyDefined(std::shared_ptr<Location> location, const std::string &name);
    void markErrorInvalidConstraints(std::shared_ptr<Location> location, const std::string &functionName, const std::string &constraints);
    void markErrorInvalidAssignment(std::shared_ptr<Location> location);
    void markErrorInvalidBuiltIn(std::shared_ptr<Location> location, const std::string &name);
    void markErrorInvalidCast(std::shared_ptr<Location> location);
    void markErrorInvalidConstant(std::shared_ptr<Location> location);
    void markErrorInvalidGlobal(std::shared_ptr<Location> location);
    void markErrorInvalidImport(std::shared_ptr<Location> location, const std::string &moduleName);
    void markErrorInvalidLiteral(std::shared_ptr<Location> location, std::shared_ptr<ValueType> type);
    void markErrorInvalidMember(std::shared_ptr<Location> location, const std::string &blobName, const std::string &memberName);
    void markErrorInvalidOperationBinary(std::shared_ptr<Location> location, ExpressionBinaryOperation operation, std::shared_ptr<ValueType> firstType, std::shared_ptr<ValueType> secondType);
    void markErrorInvalidOperationUnary(std::shared_ptr<Location> location, ExpressionUnaryOperation operation, std::shared_ptr<ValueType> type);
    void markErrorInvalidType(std::shared_ptr<Location> location);
    void markErrorUnexpected(std::shared_ptr<Location> location, const std::string &name);
    void markErrorNotDeclared(std::shared_ptr<Location> location, const std::string &name);
    void markErrorNotDefined(std::shared_ptr<Location> location, const std::string &name);
    void markErrorNoTypeForPointer(std::shared_ptr<Location> location);

    void debugPrint(const std::vector<llvm::Value *> &values);
    void debugPrint(const std::vector<llvm::Type *> &types);
};

#endif