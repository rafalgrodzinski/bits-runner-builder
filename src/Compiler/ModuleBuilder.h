#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include <map>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>

#include "Parser/Expression.h"
#include "Parser/Statement.h"

using namespace std;

class ModuleBuilder {
private:
    string moduleName;
    string sourceFileName;

    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::Type *typeVoid;
    llvm::Type *typeBool;
    llvm::IntegerType *typeSint32;
    llvm::Type *typeReal32;

    vector<shared_ptr<Statement>> statements;
    map<string, llvm::AllocaInst*> allocaMap;
    map<string, llvm::Function*> funMap;

    void buildStatement(shared_ptr<Statement> statement);
    void buildFunctionDeclaration(shared_ptr<StatementFunctionDeclaration> statement);
    void buildVarDeclaration(shared_ptr<StatementVarDeclaration> statement);
    void buildBlock(shared_ptr<StatementBlock> statement);
    void buildReturn(shared_ptr<StatementReturn> statement);
    void buildMetaExternFunction(shared_ptr<StatementMetaExternFunction> statement);
    void buildExpression(shared_ptr<StatementExpression> statement);

    llvm::Value *valueForExpression(shared_ptr<Expression> expression);
    llvm::Value *valueForLiteral(shared_ptr<ExpressionLiteral> expression);
    llvm::Value *valueForGrouping(shared_ptr<ExpressionGrouping> expression);
    llvm::Value *valueForBinary(shared_ptr<ExpressionBinary> expression);
    llvm::Value *valueForBinaryBool(ExpressionBinary::Operation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinaryInteger(ExpressionBinary::Operation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinaryReal(ExpressionBinary::Operation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForIfElse(shared_ptr<ExpressionIfElse> expression);
    llvm::Value *valueForVar(shared_ptr<ExpressionVar> expression);
    llvm::Value *valueForCall(shared_ptr<ExpressionCall> expression);

    llvm::Type *typeForValueType(ValueType valueType);
    void failWithMessage(string message);

public:
    ModuleBuilder(string moduleName, string sourceFileName, vector<shared_ptr<Statement>> statements);
    shared_ptr<llvm::Module> getModule();
};

#endif