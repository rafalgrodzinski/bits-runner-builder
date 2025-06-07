#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

#include "Expression.h"
#include "Statement.h"

using namespace std;

class ModuleBuilder {
/*private:
    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::Type *typeVoid;
    llvm::Type *typeBool;
    llvm::IntegerType *typeSInt32;
    llvm::Type *typeReal32;

    vector<shared_ptr<Statement>> statements;

    void buildStatement(shared_ptr<Statement> statement);
    void buildFunctionDeclaration(shared_ptr<StatementFunctionDeclaration> statement);
    void buildBlock(shared_ptr<StatementBlock> statement);
    void buildReturn(shared_ptr<StatementReturn> statement);
    void buildExpression(shared_ptr<StatementExpression> statement);

    llvm::Value *valueForExpression(shared_ptr<Expression> expression);
    llvm::Value *valueForLiteral(shared_ptr<ExpressionLiteral> expression);
    llvm::Value *valueForGrouping(shared_ptr<ExpressionGrouping> expression);
    llvm::Value *valueForBinary(shared_ptr<ExpressionBinary> expression);
    llvm::Value *valueForIfElse(shared_ptr<ExpressionIfElse> expression);

    llvm::Type *typeForExpression(shared_ptr<Expression> expression);

public:
    ModuleBuilder(vector<shared_ptr<Statement>> statements);
    shared_ptr<llvm::Module> getModule();*/
};

#endif